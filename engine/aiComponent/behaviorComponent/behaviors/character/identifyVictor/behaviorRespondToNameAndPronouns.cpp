/**
 * File: behaviorRespondToName.cpp
 *
 * Author: Andrew Stein -- Modified by Emily
 * Created: 12/13/2016 -- Modified by Emily: 12/19/2025
 *
 * Description: Behavior for responding to the robot being renamed
 *
 * Copyright: Anki, Inc. 2016, Emily 2025
 *
 **/

#include "engine/aiComponent/behaviorComponent/behaviors/character/identifyVictor/behaviorRespondToNameAndPronouns.h"

#include "aiComponent/behaviorComponent/behaviorExternalInterface/beiRobotInfo.h"
#include "clad/externalInterface/messageEngineToGame.h"
#include "engine/actions/basicActions.h"
#include "engine/actions/sayTextAction.h"
#include "engine/aiComponent/behaviorComponent/userIntentComponent.h"
#include "engine/aiComponent/behaviorComponent/userIntents.h"
#include "engine/components/backpackLights/engineBackpackLightComponent.h"
#include "engine/components/localeComponent.h"


namespace Anki {
namespace Vector {
  
namespace JsonKeys {

}

namespace LocalizationKey {
  const char * kImX = "BehaviorRespondToName.ImX";
}
  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRespondToNameAndPronouns::BehaviorRespondToNameAndPronouns(const Json::Value& config)
: ICozmoBehavior(config)
{
  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToNameAndPronouns::GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const
{

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToNameAndPronouns::HandleWhileInScopeButNotActivated(const EngineToGameEvent& event)
{
  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BehaviorRespondToNameAndPronouns::WantsToBeActivatedBehavior() const
{
  auto& uic = GetBehaviorComp<UserIntentComponent>();
  return uic.IsUserIntentPending(USER_INTENT(name_victor_setname))
  || uic.IsUserIntentPending(USER_INTENT(name_victor_sayname))
  || uic.IsUserIntentPending(USER_INTENT(name_victor_setpronouns))
  || uic.IsUserIntentPending(USER_INTENT(name_victor_saypronouns));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToNameAndPronouns::OnBehaviorActivated()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  UserIntentPtr intentDataSet = uic.GetUserIntentIfActive(USER_INTENT(name_victor_setname));
  UserIntentPtr intentDataSay = uic.GetUserIntentIfActive(USER_INTENT(name_victor_sayname));
  UserIntentPtr intentDataSetPronouns = uic.GetUserIntentIfActive(USER_INTENT(name_victor_setpronouns));
  UserIntentPtr intentDataSayPronouns = uic.GetUserIntentIfActive(USER_INTENT(name_victor_saypronouns));

  if (!intentDataSet && !intentDataSay && !intentDataSetPronouns && !intentDataSayPronouns) {
    PRINT_NAMED_WARNING("BehaviorRespondToNameAndPronouns.OnBehaviorActivated", "No pending 'name_victor_say' intent found");
    return;
  }

  if (intentDataSet || intentDataSay) {
    RespondToName();
  } else if (intentDataSetPronouns || intentDataSayPronouns) {
    RespondToPronouns();
  } else {
    CancelSelf();
  }

  _name.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToNameAndPronouns::RespondToName()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  UserIntentPtr intentDataSet = uic.GetUserIntentIfActive(USER_INTENT(name_victor_setname));
  UserIntentPtr intentDataSay = uic.GetUserIntentIfActive(USER_INTENT(name_victor_sayname));

  if (intentDataSet) {
    _isSetNameVc = true;
  } else if (intentDataSay) {
    _isSetNameVc = false;
  }

  // Log that the behavior was activated
  PRINT_NAMED_INFO("BehaviorRespondToNameAndPronouns.OnBehaviorActivated", "Activated naming behavior");

  if (Util::FileUtils::FileExists("/data/data/rebuild/customBotName")) {
    _name = Util::FileUtils::ReadFile("/data/data/rebuild/customBotName");
  } else if (Util::FileUtils::FileExists("/data/data/customBotName")) {
    _name = Util::FileUtils::ReadFile("/data/data/customBotName");
  } else {
    _name = "Vector";
  }

  if (_name.empty())
  {
    // The only case this can happen is if the custom name file IS made, but is blank,
    // for that case, we'll default to `Vector`, just like above
    _name = "Vector";
    if (_name.empty())
    {
      // Now, we should NEVER reach this point where the name is STILL empty because we forcefully set it above.
      // If we somehow do. we'll restore the original logic.
      PRINT_NAMED_ERROR("BehaviorRespondToNameAndPronouns.InitInternal.EmptyName", "");
      return;
    }
  }
  
  auto* action = new CompoundActionSequential();
  if (_isSetNameVc) {
    {
      // 1. Say name once (If this is setname)
      SayTextAction* sayNameAction1 = new SayTextAction(_name + "?");
      sayNameAction1->SetAnimationTrigger(AnimationTrigger::MeetVictorSayName);
      action->AddAction(sayNameAction1);
    }
  }
  
  const auto& localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();
  const std::string & localizedImName = localeComponent.GetString(LocalizationKey::kImX, _name);

  {
    // 2. Repeat name (Or say it once if not setname)
    SayTextAction* sayNameAction2 = _isSetNameVc ? new SayTextAction(_name) : new SayTextAction(localizedImName);
    _isSetNameVc ? sayNameAction2->SetAnimationTrigger(AnimationTrigger::MeetVictorSayNameAgain) : sayNameAction2->SetAnimationTrigger(AnimationTrigger::InteractWithFacesInitialNamed);
    action->AddAction(sayNameAction2);
  }
  
  DelegateIfInControl(action);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToNameAndPronouns::RespondToPronouns()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  UserIntentPtr intentDataSet = uic.GetUserIntentIfActive(USER_INTENT(name_victor_setpronouns));
  UserIntentPtr intentDataSay = uic.GetUserIntentIfActive(USER_INTENT(name_victor_saypronouns));

  if (intentDataSet) {
    _isSetPronounVc = true;
  } else if (intentDataSay) {
    _isSetPronounVc = false;
  }

  // Log that the behavior was activated
  PRINT_NAMED_INFO("BehaviorRespondToNameAndPronouns.OnBehaviorActivated", "Activated pronoun behavior");

  if (Util::FileUtils::FileExists("/data/data/rebuild/customBotPronouns")) {
    _name = Util::FileUtils::ReadFile("/data/data/rebuild/customBotPronouns");
  } else if (Util::FileUtils::FileExists("/data/data/customBotPronouns")) {
    _name = Util::FileUtils::ReadFile("/data/data/customBotPronouns");
  } else {
    _name = "he/him";
  }

  if (_name.empty())
  {
    // The only case this can happen is if the custom pronoun file IS made, but is blank,
    // for that case, we'll default to `he/him`, just like above
    _name = "he/him";
    if (_name.empty())
    {
      // Now, we should NEVER reach this point where the name is STILL empty because we forcefully set it above.
      // If we somehow do. we'll restore the original logic.
      PRINT_NAMED_ERROR("BehaviorRespondToNameAndPronouns.InitInternal.EmptyPronouns", "");
      return;
    }
  }

  if (_name == "Any!") {
    _name = "Any Pronouns!";
  } else if (_name == "He/Him") {
    _name = "";
  }

  auto* action = new CompoundActionSequential();
  if (_isSetPronounVc) {
    {
      // 1. Say name once (If this is setname)
      SayTextAction* sayNameAction1 = new SayTextAction(_name + "?");
      sayNameAction1->SetAnimationTrigger(AnimationTrigger::MeetVictorSayName);
      action->AddAction(sayNameAction1);
    }
  }

  {
    // 2. Repeat name (Or say it once if not setname)
    SayTextAction* sayNameAction2 = new SayTextAction(_name);
    _isSetPronounVc ? sayNameAction2->SetAnimationTrigger(AnimationTrigger::MeetVictorSayNameAgain) : sayNameAction2->SetAnimationTrigger(AnimationTrigger::InteractWithFacesInitialNamed);
    action->AddAction(sayNameAction2);
  }

  DelegateIfInControl(action);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToNameAndPronouns::OnBehaviorDeactivated()
{
  auto& blc = GetBEI().GetBackpackLightComponent();
  blc.ClearAllBackpackLightConfigs();
}


} // namespace Vector
} // namespace Anki
