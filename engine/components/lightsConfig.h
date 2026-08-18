/**
 * File: lightsConfig.h
 *
 * Author: Emily
 *
 * Created: 21/09/2025
 *
 * Description:
 *   Choose backpack lights thing
 *
 * Copyright: Emily, 2025
 **/

#ifndef ANKI_VECTOR_LIGHTS_CONFIG_H
#define ANKI_VECTOR_LIGHTS_CONFIG_H

#include "engine/components/rebuildConfig.h"
#include <sys/stat.h>

namespace Anki {
namespace Vector {

  inline bool& _wireoslights() {
    static bool initialized = false;
    static bool value = false;

    if (!initialized) {
      value = RebuildToggles::GetBool("wireOSLights");
      initialized = true;
    }

    return value;
  }


  inline bool& _userlights() {
    static bool initialized = false;
    static bool value = false;

    if (!initialized) {
      struct stat buffer;
      value = (stat("/data/data/customBackpackLights/off.json", &buffer) == 0) && (stat("/data/data/customBackpackLights/cubeSpinner/purple/spinner_purple_celebration.json", &buffer) == 0);
      initialized = true;
    }

    return value;
  }

  inline bool& _blinkDotLight() {
    static bool initialized = false;
    static bool value = false;

    if (!initialized) {
      value = RebuildToggles::GetBool("blinkDotLight");
      initialized = true;
    }

    return value;
  }

  inline bool& _fadeDotLight() {
    static bool initialized = false;
    static bool value = false;

    if (!initialized) {
      value = RebuildToggles::GetBool("fadeDotLight");
      initialized = true;
    }

    return value;
  }

}
}

#endif // ANKI_VECTOR_LIGHTS_CONFIG_H
