/**
 * File: neuralNetModel_tflite.cpp
 *
 * Author: Andrew Stein
 * Date:   12/5/2017
 *
 * Description: Implementation of ObjectDetector Model class which wraps TensorFlow Lite.
 *
 * Copyright: Anki, Inc. 2017
 **/

// NOTE: this wrapper completely compiles out if we're using a different model (e.g. TensorFlow)
#ifdef ANKI_NEURALNETS_USE_TFLITE

#include "coretech/neuralnets/neuralNetModel_tflite.h"
#include "coretech/vision/engine/image_impl.h"
#include <list>
#include <queue>
#include <sys/stat.h>

#include "util/fileUtils/fileUtils.h"
#include "util/logging/logging.h"

#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "tensorflow/lite/model.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/interpreter_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/profiling/profiler.h"
#include "tensorflow/lite/profiling/profile_buffer.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

namespace Anki {
namespace NeuralNets {

#define LOG_CHANNEL "NeuralNets"

// TODO: Make this a parameter in config?
constexpr int kNumThreads = 1;

namespace {
  TfLiteDelegate* gpu_delegate = nullptr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct TFLiteLogReporter : public tflite::ErrorReporter {
  int Report(const char* format, va_list args) override
  {
    constexpr size_t kMaxStringBufferSize = 1024;
    char buf[kMaxStringBufferSize];
    vsnprintf(buf, sizeof(buf), format, args);
    LOG_ERROR("TFLiteLogReporter.Report", "%s", buf);
    return 0;
  }
};

TFLiteLogReporter gLogReporter;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TFLiteModel::TFLiteModel() = default;

TFLiteModel::~TFLiteModel()
{
  if (gpu_delegate) {
    TfLiteGpuDelegateV2Delete(gpu_delegate);
    gpu_delegate = nullptr;
  }
  LOG_DEBUG("TFLiteModel.Destructor", "");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Result TFLiteModel::LoadModelInternal(const std::string& modelPath, const Json::Value& config)
{
  const Result paramsResult = _params.SetFromConfig(config);
  if(RESULT_OK != paramsResult)
  {
    return paramsResult;
  }

  DEV_ASSERT(!modelPath.empty(), "TFLiteModel.LoadModelInternal.EmptyModelPath");

  std::vector<int> sizes = {1, _params.inputHeight, _params.inputWidth, 3};

  const std::string graphFileName = Util::FileUtils::FullFilePath({modelPath,_params.graphFile});

  if(!Util::FileUtils::FileExists(graphFileName))
  {
    LOG_ERROR("TFLiteModel.LoadModelInternal.GraphFileDoesNotExist", "%s", graphFileName.c_str());
    return RESULT_FAIL;
  }

  _model = tflite::FlatBufferModel::BuildFromFile(graphFileName.c_str());

  if (!_model)
  {
    LOG_ERROR("TFLiteModel.LoadModelInternal.FailedToBuildFromFile", "%s", graphFileName.c_str());
    return RESULT_FAIL;
  }

  LOG_INFO("TFLiteModel.LoadModelInternal.Success", "Loaded: %s",
           graphFileName.c_str());

#ifdef TFLITE_CUSTOM_OPS_HEADER
  tflite::MutableOpResolver resolver;
  RegisterSelectedOps(&resolver);
#else
  tflite::ops::builtin::BuiltinOpResolver resolver;
#endif

  tflite::InterpreterBuilder(*_model, resolver)(&_interpreter);
  if (!_interpreter)
  {
    LOG_ERROR("TFLiteModel.LoadModelInternal.FailedToConstructInterpreter", "");
    return RESULT_FAIL;
  }

  if (kNumThreads != -1)
  {
    _interpreter->SetNumThreads(kNumThreads);
  }

//  mkdir("/data/tflite-cache", 0755);

  // wire
  // {
  //   TfLiteGpuDelegateOptionsV2 gpu_opts = TfLiteGpuDelegateOptionsV2Default();
  //   gpu_opts.inference_preference = TFLITE_GPU_INFERENCE_PREFERENCE_SUSTAINED_SPEED;
  //   gpu_opts.inference_priority1 = TFLITE_GPU_INFERENCE_PRIORITY_MIN_LATENCY;
  //   gpu_opts.inference_priority2 = TFLITE_GPU_INFERENCE_PRIORITY_AUTO;
  //   gpu_opts.inference_priority3 = TFLITE_GPU_INFERENCE_PRIORITY_AUTO;
  //   // gpu_opts.experimental_flags = TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_SERIALIZATION;
  //   gpu_opts.max_delegated_partitions = 1;
  //   // gpu_opts.serialization_dir = "/data/tflite-cache/";
  //   // gpu_opts.model_token = "anki";
  //   gpu_delegate = TfLiteGpuDelegateV2Create(&gpu_opts);
  //   if (gpu_delegate && !_interpreter->ModifyGraphWithDelegate(gpu_delegate)) {
  //     LOG_ERROR("TFLiteModel.LoadModelInternal.GPUDelegateFailureFrownyFace", "");
  //   }
  // }
  // wireend

  const int input = _interpreter->inputs()[0];
  _interpreter->ResizeInputTensor(input, sizes);

  if (_interpreter->AllocateTensors() != kTfLiteOk)
  {
    LOG_ERROR("TFLiteModel.LoadModelInternal.FailedToAllocateTensors", "");
    return RESULT_FAIL;
  }

  // Read the label list
  const std::string labelsFileName = Util::FileUtils::FullFilePath({modelPath, _params.labelsFile});
  const Result readLabelsResult = ReadLabelsFile(labelsFileName, _labels);
  if(RESULT_OK != readLabelsResult)
  {
    return readLabelsResult;
  }

  return RESULT_OK;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TFLiteModel::ScaleImage(Vision::ImageRGB& img)
{
  DEV_ASSERT(_interpreter != nullptr, "TFLiteModel.ScaleImage.NullInterpreter");

  const int inputIndex = _interpreter->inputs()[0];

  const auto kResizeMethod = Vision::ResizeMethod::Linear;

  if(_params.useFloatInput)
  {
    float* scaledInputData = _interpreter->typed_tensor<float>(inputIndex);
    DEV_ASSERT(scaledInputData != nullptr, "TFLiteModel.ScaleImage.NullInputData");

    // Resize uint8 image data, and *then* convert smaller image to float below
    // TODO: Resize and convert directly into the scaledInputData (ideally using NEON?)
    if(img.GetNumRows() != _params.inputHeight || img.GetNumCols() != _params.inputWidth)
    {
      img.Resize(_params.inputHeight, _params.inputWidth, kResizeMethod);
    }
    else if(_params.verbose)
    {
      LOG_INFO("TFLiteModel.ScaleImage.SkipResize", "Skipping actual resize: image already correct size");
    }
    DEV_ASSERT(img.IsContinuous(), "TFLiteModel.ScaleImage.ImageNotContinuous");

    // Scale/shift resized image directly into the tensor data
    DEV_ASSERT(img.GetNumChannels() == 3, "TFLiteModel.ScaleImage.BadNumChannels");

    cv::Mat cvTensor(_params.inputHeight, _params.inputWidth, CV_32FC3, scaledInputData);

    img.get_CvMat_().convertTo(cvTensor, CV_32FC3, 1.f/_params.inputScale, _params.inputShift);
  }
  else
  {
    // Resize uint8 input image directly into the uint8 tensor data by wrapping image "header"
    // around the tensor's data buffer
    uint8_t* scaledInputData = _interpreter->typed_tensor<uint8_t>(inputIndex);
    Vision::ImageRGB tensorImg(_params.inputHeight, _params.inputWidth, scaledInputData);
    img.Resize(tensorImg, kResizeMethod);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Result TFLiteModel::Detect(Vision::ImageRGB& img, std::list<Vision::SalientPoint>& salientPoints)
{
  // Scale image, subtract mean, divide by standard deviation and store in the interpreter's input tensor
  ScaleImage(img);

  const auto invokeResult = _interpreter->Invoke();
  if (kTfLiteOk != invokeResult)
  {
    LOG_ERROR("TFLiteModel.Detect.FailedToInvoke", "");
    return RESULT_FAIL;
  }

  switch(_params.outputType)
  {
    case NeuralNetParams::OutputType::Classification:
    {
      if(_params.useFloatInput)
      {
        const float* outputData = _interpreter->typed_output_tensor<float>(0);
        ClassificationOutputHelper(outputData, img.GetTimestamp(), salientPoints);
      }
      else
      {
        const uint8_t* outputData = _interpreter->typed_output_tensor<uint8_t>(0);
        ClassificationOutputHelper(outputData, img.GetTimestamp(), salientPoints);
      }

      break;
    }

    case NeuralNetParams::OutputType::BinaryLocalization:
    {
      if(_params.useFloatInput)
      {
        const float* outputData = _interpreter->typed_output_tensor<float>(0);
        LocalizedBinaryOutputHelper(outputData, img.GetTimestamp(),  1.f, 0, salientPoints);
      }
      else
      {
        const uint8_t* outputData = _interpreter->typed_output_tensor<uint8_t>(0);
        const std::vector<int>& outputs_ = _interpreter->outputs();
        TfLiteTensor* outputTensor = _interpreter->tensor(outputs_[0]);
        const float scale = outputTensor->params.scale;
        const int zero_point = outputTensor->params.zero_point;
        LocalizedBinaryOutputHelper(outputData, img.GetTimestamp(), scale, zero_point, salientPoints);
      }

      break;
    }

    case NeuralNetParams::OutputType::AnchorBoxes:
      //GetDetectedObjects(outputTensors, t, salientPoints);
    case NeuralNetParams::OutputType::Segmentation:
    {
      LOG_ERROR("TFLiteModel.Detect.OutputTypeNotSupported",
                "TFLite model needs more output types implemented");
      return RESULT_FAIL;
    }
  }

  return RESULT_OK;
}

} // namespace Vision
} // namespace Anki

#endif // #if ANKI_NEURALNETS_USE_TFLITE
