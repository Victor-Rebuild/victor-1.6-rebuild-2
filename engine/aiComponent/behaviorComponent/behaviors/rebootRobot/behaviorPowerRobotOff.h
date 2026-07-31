/**
 * File: BehaviorPowerRobotOff.cpp
 *
 * Author: Emily Modder
 * Created: 2026-03-14
 *
 * Description: Behavior which powers off or reboots the robot on a vc
 *
 * Copyright: Victor-Rebuild, 2026
 *
 **/

#ifndef __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorPowerRobotOff__
#define __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorPowerRobotOff__
#pragma once

#include "engine/aiComponent/behaviorComponent/behaviors/iCozmoBehavior.h"

namespace Anki {
namespace Vector {

// forward declaration
class IBEICondition;

class BehaviorPowerRobotOff : public ICozmoBehavior
{
public:
  virtual ~BehaviorPowerRobotOff();

protected:

  // Enforce creation through BehaviorFactory
  friend class BehaviorFactory;
  explicit BehaviorPowerRobotOff(const Json::Value& config);

  virtual void GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const override;
  virtual void GetAllDelegates(std::set<IBehavior*>& delegates) const override;
  virtual void GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const override;

  virtual void InitBehavior() override;
  virtual bool WantsToBeActivatedBehavior() const override;
  virtual void OnBehaviorActivated() override;
  virtual void BehaviorUpdate() override;
  virtual void OnBehaviorLeftActivatableScope() override;

  virtual void AlwaysHandleInScope(const RobotToEngineEvent& event) override;


private:
  struct InstanceConfig {
    InstanceConfig(const Json::Value& config);
    std::shared_ptr<IBEICondition> activateBehaviorCondition;
    std::string powerOffAnimName;

    std::string findChargerBehaviorName;
    ICozmoBehaviorPtr findChargerBehavior;
  };

  struct DynamicVariables {
    DynamicVariables();
    bool waitingForAnimationCallback;
    TimeStamp_t timeLastPowerAnimStopped_ms;
    std::string lastAnimPlayedName;
    bool shouldStartPowerOffAnimaiton;
    bool isShutdown;
  };

  InstanceConfig _iConfig;
  DynamicVariables _dVars;

  void TransitionToCharger();

  void TransitionToPoweringOff();

  void StartAnimation(const std::string& animName, const TimeStamp_t startTime_ms);
  TimeStamp_t GetLengthOfAnimation_ms(const std::string& animName);

  bool isPowerOff = false;

};

} // namespace Vector
} // namespace Anki

#endif // __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorPowerRobotOff__
