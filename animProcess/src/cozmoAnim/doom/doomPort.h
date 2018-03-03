
#include <thread>

namespace Anki{
namespace Vision {
  class ImageRGB565;
}


  namespace Vector {
    struct RobotState;
    namespace Audio {
      class CozmoAudioController;
    }
  }

}

class DoomPort {
public:
  DoomPort(const std::string& resourcePath, unsigned int width, unsigned int height);

  using AudioController = Anki::Vector::Audio::CozmoAudioController;
  void SetAudioController( AudioController* ac );

  void Run();
  void Stop();

  void GetScreen(Anki::Vision::ImageRGB565& screen);

  void HandleMessage(const Anki::Vector::RobotState& robotState);

  void Update();

  std::exception_ptr GetException();
private:

  void CallDoomMain();
  std::thread _thread;

  std::mutex _mutex;
  std::exception_ptr _exception;

  float _robotAccelYFiltered = 0.0f;

};
