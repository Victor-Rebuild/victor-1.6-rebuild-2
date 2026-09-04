#include "coretech/common/shared/array2d_impl.h"
#include "coretech/vision/engine/colorPixelTypes.h"

#include <iostream>

namespace Anki
{
  // Force there to be an instantiatino of the templated class with some
  // type, so we can see if it compiles
  void testArray2dInstantiation(void)
  {
    Array2d<float> stuff(100,100);

    // Exponentiate "stuff" in place:
    stuff.ApplyScalarFunction(expf);
  }

  // PixelRGB565
  template Vision::PixelRGB565*       Array2d<Vision::PixelRGB565>::GetRow(s32);
  template const Vision::PixelRGB565* Array2d<Vision::PixelRGB565>::GetRow(s32) const;

  // PixelRGBA
  template Vision::PixelRGBA*       Array2d<Vision::PixelRGBA>::GetRow(s32);
  template const Vision::PixelRGBA* Array2d<Vision::PixelRGBA>::GetRow(s32) const;
  template s32 Array2d<Vision::PixelRGBA>::GetNumRows() const;
  template s32 Array2d<Vision::PixelRGBA>::GetNumCols() const;
  template cv::Mat_<Vision::PixelRGBA>&       Array2d<Vision::PixelRGBA>::get_CvMat_();
  template const cv::Mat_<Vision::PixelRGBA>& Array2d<Vision::PixelRGBA>::get_CvMat_() const;
} // namespace Anki
