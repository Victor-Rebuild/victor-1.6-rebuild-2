/**
 * File: rebuildConfig.h
 *
 * Author: Emily
 *
 * Created: 08/16/2026
 *
 * Description:
 *   Thing that manages settings for 1.6-rebuild.
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
constexpr const char* kRebuildGameStatsFile = "/data/data/rebuild/rebuildGameStats.json";

inline bool GetBool(const std::string& key, bool defaultVal = false, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asBool();
}

inline std::string GetString(const std::string& key, const std::string& defaultVal = "", bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asString();
}

inline float GetFloat(const std::string& key, float defaultVal = -1, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asFloat();
}

inline int GetInt(const std::string& key, int defaultVal = -1, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  return toggles.get(key, defaultVal).asInt();
}

inline void SetBool(Util::Data::DataPlatform* platform, const std::string& key, bool val, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
}

inline void SetString(Util::Data::DataPlatform* platform, const std::string& key, const std::string& val, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
}

inline void SetFloat(Util::Data::DataPlatform* platform, const std::string& key, float val = -1, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
}

inline void SetInt(Util::Data::DataPlatform* platform, const std::string& key, int val = -1, bool gameStat = false) {
  Json::Value toggles;
  Util::Data::DataPlatform::readAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
  toggles[key] = val;
  platform->writeAsJson(gameStat ? kRebuildGameStatsFile :  kRebuildSettingsFile, toggles);
}

} // namespace RebuildToggles
} // namespace Vector
} // namespace Anki