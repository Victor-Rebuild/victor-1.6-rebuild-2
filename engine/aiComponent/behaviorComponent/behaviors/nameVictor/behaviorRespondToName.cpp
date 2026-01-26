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

#include "engine/aiComponent/behaviorComponent/behaviors/nameVictor/behaviorRespondToName.h"

#include "aiComponent/behaviorComponent/behaviorExternalInterface/beiRobotInfo.h"
#include "clad/externalInterface/messageEngineToGame.h"
#include "engine/actions/basicActions.h"
#include "engine/actions/sayTextAction.h"
#include "engine/aiComponent/behaviorComponent/userIntentComponent.h"
#include "engine/aiComponent/behaviorComponent/userIntents.h"
#include "engine/components/backpackLights/engineBackpackLightComponent.h"
#include "engine/components/localeComponent.h"
#include "engine/events/ankiEvent.h"
#include "engine/externalInterface/externalInterface.h"
#include "util/cladHelpers/cladFromJSONHelpers.h"


namespace Anki {
namespace Vector {
  
namespace JsonKeys {
  // static const char * const AnimationTriggerKey = "animationTrigger";
}

namespace LocalizationKey {
  const char * kImX = "BehaviorRespondToName.ImX";
}
  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRespondToName::BehaviorRespondToName(const Json::Value& config)
: ICozmoBehavior(config)
, _name("")
{
  SubscribeToTags({EngineToGameTag::RobotRenamedEnrolledFace});
  
  //  const std::string& animTriggerString = config.get(JsonKeys::AnimationTriggerKey, "MeetCozmoRenameFaceSayName").asString();
  //  _animTrigger = AnimationTriggerFromString(animTriggerString.c_str());
  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToName::GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const
{
  //  const char* list[] = {
  //    JsonKeys::AnimationTriggerKey,
  //  };
  //  expectedKeys.insert( std::begin(list), std::end(list) );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToName::HandleWhileInScopeButNotActivated(const EngineToGameEvent& event)
{
  
  auto & msg = event.GetData().Get_RobotRenamedEnrolledFace();
  _name   = msg.name;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BehaviorRespondToName::WantsToBeActivatedBehavior() const
{
  auto& uic = GetBehaviorComp<UserIntentComponent>();
  return uic.IsUserIntentPending(USER_INTENT(name_victor_setname)) || uic.IsUserIntentPending(USER_INTENT(name_victor_sayname));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToName::OnBehaviorActivated()
{
  // The intent code was used from wireOS (https://github.com/kercre123/victor/blob/snowboy/engine/aiComponent/behaviorComponent/behaviors/victor/behaviorWireTest.cpp)
  UserIntentPtr intentDataSet = SmartActivateUserIntent(USER_INTENT(name_victor_setname));
  UserIntentPtr intentDataSay = SmartActivateUserIntent(USER_INTENT(name_victor_sayname));

  if (!intentDataSet && !intentDataSay) {
    PRINT_NAMED_WARNING("BehaviorRespondToName.OnBehaviorActivated", "No pending 'name_victor_say' intent found");
    return;
  }
  
  if (intentDataSet) {
    isSetNameVc = 1;
  } else if (intentDataSay) {
    isSetNameVc = 0;
  }

  // Log that the behavior was activated
  PRINT_NAMED_INFO("BehaviorRespondToName.OnBehaviorActivated", "Activated 'name_victor_say' intent");

  if (Util::FileUtils::FileExists("/data/data/customBotName")) {
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
      PRINT_NAMED_ERROR("BehaviorRespondToName.InitInternal.EmptyName", "");
      return;
    }
  }
  
  //  PRINT_CH_INFO("Behaviors", "BehaviorRespondToName.InitInternal",
  //                "Responding to rename of %s with %s",
  //                Util::HidePersonallyIdentifiableInfo(_name.c_str()),
  //                EnumToString(_animTrigger));
  
  // TODO: Try to turn towards a/the face first COZMO-7991
  //  For some reason the following didn't work (action immediately completed) and I ran
  //  out of time figuring out why. I also tried simply turning towards last face pose with
  //  no luck.
  //
  //  const bool kSayName = true;
  //  TurnTowardsFaceAction* turnTowardsFace = new TurnTowardsLastFacePoseAction(_faceID, M_PI_F, kSayName);
  //
  //  // Play the animation trigger whether or not we find the face
  //  turnTowardsFace->SetSayNameAnimationTrigger(_animTrigger);
  //  turnTowardsFace->SetNoNameAnimationTrigger(_animTrigger);
  //  
  //  DelegateIfInControl(turnTowardsFace);
  
  auto* action = new CompoundActionSequential();
  if (isSetNameVc) {
    {
      // 1. Say name once (If this is setname)
      SayTextAction* sayNameAction1 = new SayTextAction(_name + "?");
      sayNameAction1->SetAnimationTrigger(AnimationTrigger::MeetVictorSayName);
      action->AddAction(sayNameAction1);
    }
  }
  
  const std::string & localizedImName = GetLocalizedImX();

  {
    // 2. Repeat name (Or say it once if not setname)
    SayTextAction* sayNameAction2 = isSetNameVc ? new SayTextAction(_name) : new SayTextAction(localizedImName + _name);
    isSetNameVc ? sayNameAction2->SetAnimationTrigger(AnimationTrigger::MeetVictorSayNameAgain) : sayNameAction2->SetAnimationTrigger(AnimationTrigger::InteractWithFacesInitialNamed);
    action->AddAction(sayNameAction2);
  }
  
  DelegateIfInControl(action);
  
  _name.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRespondToName::OnBehaviorDeactivated()
{
  auto& blc = GetBEI().GetBackpackLightComponent();
  blc.ClearAllBackpackLightConfigs();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Get localized version of "I'm, X"
std::string BehaviorRespondToName::GetLocalizedImX() const
{
  return GetLocalizedString(LocalizationKey::kImX);
}

std::string BehaviorRespondToName::GetLocalizedString(const std::string & key) const
{
  const auto& localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();
  return localeComponent.GetString(key);
}


} // namespace Vector
} // namespace Anki
