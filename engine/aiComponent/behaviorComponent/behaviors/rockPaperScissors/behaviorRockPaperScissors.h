/**
 * File: BehaviorRockPaperScissors.cpp
 *
 * Author: Emily Modder
 * Created: 2026-03-14
 *
 * Description: Behavior which powers off or reboots the robot on a vc
 *
 * Copyright: Victor-Rebuild, 2026
 *
 **/

#ifndef __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorRockPaperScissors__
#define __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorRockPaperScissors__
#pragma once

#include "engine/aiComponent/behaviorComponent/behaviors/iCozmoBehavior.h"
#include "engine/aiComponent/behaviorComponent/behaviors/animationWrappers/behaviorTextToSpeechLoop.h"
#include "engine/aiComponent/behaviorComponent/behaviors/robotDrivenDialog/behaviorPromptUserForVoiceCommand.h"

namespace Anki {
namespace Vector {

// forward declaration
class IBEICondition;

class BehaviorRockPaperScissors : public ICozmoBehavior
{
public:
  virtual ~BehaviorRockPaperScissors();

protected:

  // Enforce creation through BehaviorFactory
  friend class BehaviorFactory;
  explicit BehaviorRockPaperScissors(const Json::Value& config);

  virtual void GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const override;
  virtual void GetAllDelegates(std::set<IBehavior*>& delegates) const override;
  virtual void GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const override;

  virtual void InitBehavior() override;
  virtual bool WantsToBeActivatedBehavior() const override;
  virtual void OnBehaviorEnteredActivatableScope() override;
  virtual void OnBehaviorActivated() override;
  virtual void BehaviorUpdate() override;
  virtual void OnBehaviorLeftActivatableScope() override;

  virtual void AlwaysHandleInScope(const RobotToEngineEvent& event) override;


private:
  struct InstanceConfig {
    InstanceConfig(const Json::Value& config);
    std::shared_ptr<IBEICondition> activateBehaviorCondition;
    std::string powerOnAnimName;
    std::string powerOffAnimName;
    bool waitForAnimMsg;

    ICozmoBehaviorPtr findChargerBehavior;
    std::shared_ptr<BehaviorTextToSpeechLoop> ttsBehavior;
    std::shared_ptr<BehaviorPromptUserForVoiceCommand> hitOrStandPromptBehavior;
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

  void StartTTSInit();
  void RockPaperOrScissors();

  bool isPowerOff = false;

};

} // namespace Vector
} // namespace Anki

#endif // __Engine_AiComponent_BehaviorComponent_Behaviors_BehaviorRockPaperScissors__
