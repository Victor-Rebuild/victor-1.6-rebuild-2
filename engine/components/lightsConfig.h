/**
 * File: lightsConfig.h
 *
 * Author: Raj-jyot
 *
 * Created: 21/09/2025
 *
 * Description:
 *   Choose backpack lights thing
 *
 * Copyright: Raj-jyot, 2025
 **/

#ifndef ANKI_VECTOR_LIGHTS_CONFIG_H
#define ANKI_VECTOR_LIGHTS_CONFIG_H

#include "util/fileUtils/fileUtils.h"
#include <sys/stat.h>

namespace Anki {
namespace Vector {

  inline bool& _wireoslights() {
    static bool initialized = false;
    static bool value = false;

    if (!initialized) {
      if (Util::FileUtils::FileExists("/data/data/wirelights")) {
        value = Util::FileUtils::MoveFile("/data/data/wirelights", "/data/data/rebuild/wirelights");
        initialized = true;
      }

      value = Util::FileUtils::FileExists("/data/data/rebuild/wirelights");
      initialized = true;
    }

    return value;
  }


  inline bool& _userlights() {
    static bool initialized = false;
    static bool value = false;

    if (!initialized) {
      struct stat buffer;
      value = (stat("/data/data/customBackpackLights/off.json", &buffer) == 0);
      initialized = true;
    }

    if (!initialized) {
      value = Util::FileUtils::FileExists("/data/data/customBackpackLights/off.json") && Util::FileUtils::FileExists("/data/data/customBackpackLights/cubeSpinner/purple/spinner_purple_celebration.json");
      initialized = true;
    }

    return value;
  }

}
}

#endif // ANKI_VECTOR_LIGHTS_CONFIG_H
