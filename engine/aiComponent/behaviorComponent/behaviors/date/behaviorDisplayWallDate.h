/**
 * File: behaviorDisplayWallDate.h
 *
 * Author: Emily modder
 * Created: 2026-02-26
 *
 * Description: If the robot has a valid date, display it on the robot's face
 *
 * Copyright: Victor-Rebuild, 2026
 *
 **/

#ifndef __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorDisplayWallDate__
#define __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorDisplayWallDate__
#pragma once

#include "engine/aiComponent/behaviorComponent/behaviors/timer/behaviorProceduralClock.h"
#include "anki/cozmo/shared/cozmoConfig.h"

namespace Anki {
namespace Vector {

class BehaviorDisplayWallDate : public BehaviorProceduralClock
{
public: 
  virtual ~BehaviorDisplayWallDate();

  // override the time that is displayed the next time this behavior is called. This will automatically be
  // cleared after this behavior activated
  void SetOverrideDisplayTime(struct tm& time);

protected:
  // Enforce creation through BehaviorFactory
  friend class BehaviorFactory;
  explicit BehaviorDisplayWallDate(const Json::Value& config);

  virtual void OnBehaviorDeactivated() override;

  virtual void GetBehaviorOperationModifiersProceduralClock(BehaviorOperationModifiers& modifiers) const override;
  virtual void GetBehaviorJsonKeysInternal(std::set<const char*>& expectedKeys) const override;

  virtual void TransitionToShowClockInternal() override;
  virtual bool WantsToBeActivatedBehavior() const override;

private:

  bool _hasTimeOverride;
  struct tm _timeOverride;

  BehaviorProceduralClock::GetDigitsFunction BuildTimerFunction() const;
};

} // namespace Vector
} // namespace Anki

#endif // __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorDisplayWallDate__
