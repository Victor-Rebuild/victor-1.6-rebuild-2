/**
 * File: BehaviorDisplayWallDate.cpp
 *
 * Author: Kevin M. Karol
 * Created: 2018-05-30
 *
 * Description: If the robot has a valid wall time, display it on the robot's face
 *
 * Copyright: Anki, Inc. 2018
 *
 **/


#include "engine/aiComponent/behaviorComponent/behaviors/timer/behaviorDisplayWallDate.h"

#include "engine/aiComponent/timerUtility.h"
#include "engine/components/settingsManager.h"
#include "osState/wallTime.h"

#include "util/console/consoleInterface.h"

namespace Anki {
namespace Vector {
  

namespace{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorDisplayWallDate::BehaviorDisplayWallDate(const Json::Value& config)
: BehaviorProceduralClock(config)
{
  SetGetDigitFunction(BuildTimerFunction());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorDisplayWallDate::~BehaviorDisplayWallDate()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorDisplayWallDate::GetBehaviorOperationModifiersProceduralClock( BehaviorOperationModifiers& modifiers ) const
{
  modifiers.wantsToBeActivatedWhenOffTreads = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorDisplayWallDate::GetBehaviorJsonKeysInternal(std::set<const char*>& expectedKeys) const 
{
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorDisplayWallDate::SetOverrideDisplayTime(struct tm& time)
{
  if( _hasTimeOverride ) {
    PRINT_NAMED_WARNING("BehaviorDisplayWallDate.TimeAlreadyOverriden",
                        "attempting to overide time but there is already an overide presnt");
  }
  _timeOverride = time;
  _hasTimeOverride = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorDisplayWallDate::OnBehaviorDeactivated()
{
  _hasTimeOverride = false;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorDisplayWallDate::TransitionToShowClockInternal()
{
  BuildAndDisplayProceduralClock(0, 0);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BehaviorDisplayWallDate::WantsToBeActivatedBehavior() const
{
  // Ensure we can get local time
  struct tm unused;
  return _hasTimeOverride || WallTime::getInstance()->GetApproximateLocalTime(unused);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorProceduralClock::GetDigitsFunction BehaviorDisplayWallDate::BuildTimerFunction() const
{
  return [this](const int offset){
    std::map<Vision::SpriteBoxName, int> outMap;
    struct tm localDate;
    std::string dateResponse;

    if( _hasTimeOverride ) {
      localDate = _timeOverride;
    }
    else {
      if(!WallTime::getInstance()->GetLocalTime(localDate)){
        PRINT_NAMED_ERROR("BehaviorProceduralDate.BuildTimerFunction.NoTime",
                          "Behavior '%s' activated when it had a valid wall time, but now it does not",
                          GetDebugLabel().c_str());
        return outMap;
      }
    }
    
    // Convert to seconds
    int currentMonth = localDate.tm_mon + 1;
    int currentDay = localDate.tm_mday;

    // Tens Digit (left of colon)
    {
      const int tensDigit = currentMonth/10;
      outMap.emplace(std::make_pair(IsXray() ? Vision::SpriteBoxName::TensLeftOfColonXray : Vision::SpriteBoxName::TensLeftOfColon, tensDigit));
    }
    
    // Ones Digit (left of colon)
    {
      const int onesDigit = currentMonth % 10;
      outMap.emplace(std::make_pair(IsXray() ? Vision::SpriteBoxName::OnesLeftOfColonXray : Vision::SpriteBoxName::OnesLeftOfColon, onesDigit));
    }

    // Tens Digit (right of colon)
    {
      const int tensDigit = currentDay/10;
      outMap.emplace(std::make_pair(IsXray() ? Vision::SpriteBoxName::TensRightOfColonXray : Vision::SpriteBoxName::TensRightOfColon, tensDigit));
    }

    // Ones Digit (right of colon)
    {
      const int onesDigit = currentDay % 10;
      outMap.emplace(std::make_pair(IsXray() ? Vision::SpriteBoxName::OnesRightOfColonXray : Vision::SpriteBoxName::OnesRightOfColon, onesDigit));
    }

    return outMap;
  };
}

} // namespace Vector
} // namespace Anki
