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

#ifndef __Cozmo_Basestation_Behaviors_BehaviorRespondToName_H__
#define __Cozmo_Basestation_Behaviors_BehaviorRespondToName_H__

#include "engine/aiComponent/behaviorComponent/behaviors/iCozmoBehavior.h"

namespace Anki {
namespace Vector {
  
class BehaviorRespondToName : public ICozmoBehavior
{
private:
  
  // Enforce creation through BehaviorFactory
  friend class BehaviorFactory;
  BehaviorRespondToName(const Json::Value& config);
  
public:
  virtual bool WantsToBeActivatedBehavior() const override;
  
protected:
  virtual void GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const override {}
  virtual void GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const override;
  virtual void OnBehaviorActivated()   override;
  virtual void OnBehaviorDeactivated()   override;
  virtual void HandleWhileInScopeButNotActivated(const EngineToGameEvent& event) override;
  
private:
  
  std::string      _name;
  bool isSetNameVc = false;

  // Below is taken from behaviorenrollface.cpp/.h

  // Get localized version of "I'm, X"
  std::string GetLocalizedImX() const;

  // Get localized string "I'm, X"
  std::string GetLocalizedString(const std::string & key) const;
  std::string GetLocalizedString(const std::string & key, const std::string & arg0) const;
  std::string GetLocalizedString(const std::string & key, const std::string & arg0, const std::string & arg1) const;
  
}; // class BehaviorReactToRenameFace
  

} // namespace Vector
} // namespace Anki

#endif // __Cozmo_Basestation_Behaviors_BehaviorRespondToName_H__
