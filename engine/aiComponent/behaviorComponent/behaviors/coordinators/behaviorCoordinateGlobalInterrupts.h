/**
* File: behaviorCoordinateGlobalInterrupts.h
*
* Author: Kevin M. Karol
* Created: 2/22/18
*
* Description: Behavior responsible for handling special case needs 
* that require coordination across behavior global interrupts
*
* Copyright: Anki, Inc. 2018
*
**/

#ifndef __Engine_Behaviors_BehaviorCoordinateGlobalInterrupts_H__
#define __Engine_Behaviors_BehaviorCoordinateGlobalInterrupts_H__


#include "engine/aiComponent/behaviorComponent/behaviors/dispatch/behaviorDispatcherPassThrough.h"
#include "engine/aiComponent/behaviorComponent/behaviorTreeStateHelpers.h"

namespace Anki {
namespace Vector {

// forward declarations
class BehaviorDriveToFace;
class BehaviorHighLevelAI;
class BehaviorReactToVoiceCommand;
class BehaviorTimerUtilityCoordinator;


class BehaviorCoordinateGlobalInterrupts : public ICozmoBehavior
{
public:
  virtual ~BehaviorCoordinateGlobalInterrupts();

protected:
  // Enforce creation through BehaviorFactory
  friend class BehaviorFactory;  
  BehaviorCoordinateGlobalInterrupts(const Json::Value& config);

  virtual void GetAllDelegates(std::set<IBehavior*>& delegates) const override;

  virtual void GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const override {}
  virtual void GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const override {}

  //virtual void InitBehavior() override;
  virtual void OnBehaviorActivated() override;
  //virtual void OnBehaviorDeactivated() override;
  //virtual void BehaviorUpdate() override;
  virtual bool WantsToBeActivatedBehavior() const override;

private:
  
  void CreateConsoleVars();
  
  struct InstanceConfig{
    InstanceConfig();
  };

  struct DynamicVariables{
    DynamicVariables();

    bool suppressProx;
  };

  InstanceConfig   _iConfig;
  DynamicVariables _dVars;

  bool ShouldSuppressProxReaction();
  
};

} // namespace Vector
} // namespace Anki


#endif // __Engine_Behaviors_BehaviorCoordinateGlobalInterrupts_H__
