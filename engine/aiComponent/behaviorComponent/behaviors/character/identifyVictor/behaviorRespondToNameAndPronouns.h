/**
 * File: behaviorRespondToName.h
 *
 * Author: Andrew Stein
 * Created: 12/13/2016
 *
 * Description: Behavior for responding to a face being renamed
 *
 * Copyright: Anki, Inc. 2016
 *
 **/

#ifndef __Cozmo_Basestation_Behaviors_BehaviorRespondToNameAndPronouns_H__
#define __Cozmo_Basestation_Behaviors_BehaviorRespondToNameAndPronouns_H__

#include "engine/aiComponent/behaviorComponent/behaviors/iCozmoBehavior.h"

namespace Anki {
namespace Vector {
  
class BehaviorRespondToNameAndPronouns : public ICozmoBehavior
{
private:
  
  // Enforce creation through BehaviorFactory
  friend class BehaviorFactory;
  BehaviorRespondToNameAndPronouns(const Json::Value& config);
  
public:
  bool WantsToBeActivatedBehavior() const override;
  
protected:
  void GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const override {}
  void GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const override;
  void OnBehaviorActivated()   override;
  void RespondToName();
  void RespondToPronouns();
  void OnBehaviorDeactivated()   override;
  void HandleWhileInScopeButNotActivated(const EngineToGameEvent& event) override;
  
private:
  
  std::string      _name;
  bool _isSetNameVc = false;
  bool _isSetPronounVc = false;
  
}; // class BehaviorReactToRenameFace
  

} // namespace Vector
} // namespace Anki

#endif // __Cozmo_Basestation_Behaviors_BehaviorRespondToNameAndPronouns_H__
