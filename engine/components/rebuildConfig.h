/**
 * File: lightsConfig.h
 *
 * Author: Emily
 *
 * Created: 08/16/2025
 *
 * Description:
 *   Choose backpack lights thing
 *
 * Copyright: Emily, 2026
 **/

#pragma once
#include "coretech/common/engine/utils/data/dataPlatform.h"
#include "json/json.h"
#include <string>

namespace Anki {
namespace Vector {
namespace RebuildToggles {

constexpr const char* kRebuildSettingsFile = "/data/data/rebuild/settings.json";

inline bool GetBool(const std::string& key, bool defaultVal = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asBool();
}

inline std::string GetString(const std::string& key, const std::string& defaultVal = "") {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asString();
}

inline float GetFloat(const std::string& key, float defaultVal = -1) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asFloat();
}

inline int GetInt(const std::string& key, int defaultVal = -1) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asInt();
}

inline void SetBool(Util::Data::DataPlatform* platform, const std::string& key, bool val) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(kRebuildSettingsFile, toggles);
}

inline void SetString(Util::Data::DataPlatform* platform, const std::string& key, const std::string& val) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(kRebuildSettingsFile, toggles);
}

inline void SetFloat(Util::Data::DataPlatform* platform, const std::string& key, float val = -1) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(kRebuildSettingsFile, toggles);
}

inline void SetInt(Util::Data::DataPlatform* platform, const std::string& key, int val = -1) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(kRebuildSettingsFile, toggles);
}

} // namespace RebuildToggles
} // namespace Vector
} // namespace Anki