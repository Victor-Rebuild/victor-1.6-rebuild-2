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


#include "engine/aiComponent/behaviorComponent/behaviors/rockPaperScissors/behaviorRockPaperScissors.h"

#include "aiComponent/behaviorComponent/behaviorContainer.h"
#include "cannedAnimLib/cannedAnims/cannedAnimationContainer.h"
#include "clad/robotInterface/messageEngineToRobot.h"
#include "clad/robotInterface/messageRobotToEngine.h"
#include "engine/actions/animActions.h"
#include "engine/aiComponent/behaviorComponent/behaviorExternalInterface/beiRobotInfo.h"
#include "engine/aiComponent/behaviorComponent/userIntentComponent.h"
#include "engine/aiComponent/behaviorComponent/userIntents.h"
#include "engine/components/dataAccessorComponent.h"
#include "engine/externalInterface/externalInterface.h"

namespace Anki {
namespace Vector {

namespace{
  static const UserIntentTag scissorsIntent = USER_INTENT(play_rockPaperScissorsScissors);
  static const UserIntentTag paperIntent = USER_INTENT(play_rockPaperScissorsPaper);
  static const UserIntentTag rockIntent = USER_INTENT(play_rockPaperScissorsRock);
  static const UserIntentTag silenceIntent = USER_INTENT(silence);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::InstanceConfig::InstanceConfig(const Json::Value& config)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::DynamicVariables::DynamicVariables()
: waitingForAnimationCallback(false)
, timeLastPowerAnimStopped_ms(0)
, shouldStartPowerOffAnimaiton(false)
, isShutdown(false)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::BehaviorRockPaperScissors(const Json::Value& config)
: ICozmoBehavior(config)
, _iConfig(config)
{
  SubscribeToTags({RobotInterface::RobotToEngineTag::startShutdownAnim});
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::~BehaviorRockPaperScissors()
{
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::InitBehavior()
{
  auto& BC = GetBEI().GetBehaviorContainer();

  BC.FindBehaviorByIDAndDowncast( BEHAVIOR_ID(DefaultTextToSpeechLoop),
                                  BEHAVIOR_CLASS(TextToSpeechLoop),
                                  _iConfig.ttsBehavior );

  BC.FindBehaviorByIDAndDowncast( BEHAVIOR_ID(BlackJackHitOrStandPrompt),
                                  BEHAVIOR_CLASS(PromptUserForVoiceCommand),
                                  _iConfig.hitOrStandPromptBehavior );
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BehaviorRockPaperScissors::WantsToBeActivatedBehavior() const
{
  auto& uic = GetBehaviorComp<UserIntentComponent>();
  return uic.IsUserIntentPending(USER_INTENT(play_rockPaperScissors));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const
{
  modifiers.behaviorAlwaysDelegates = false;
  modifiers.wantsToBeActivatedWhenOffTreads = true;
  modifiers.wantsToBeActivatedWhenOnCharger = true;
  modifiers.wantsToBeActivatedWhenCarryingObject = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::GetAllDelegates(std::set<IBehavior*>& delegates) const
{
  delegates.insert( _iConfig.ttsBehavior.get() );
  delegates.insert( _iConfig.hitOrStandPromptBehavior.get() );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const
{
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::OnBehaviorEnteredActivatableScope()
{
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::OnBehaviorActivated()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  UserIntentPtr intentDataReboot   = uic.GetUserIntentIfActive(USER_INTENT(play_rockPaperScissors));
  // UserIntentPtr intentDataShutdown = uic.GetUserIntentIfActive(USER_INTENT(INVALID));
  // UserIntentPtr intentDataShutdownSatisfied = uic.GetUserIntentIfActive(USER_INTENT(INVALID));

  _dVars = DynamicVariables();

  // if (intentDataShutdown || intentDataShutdownSatisfied) {
  //   _dVars.isShutdown = true;
  // } else
  if (intentDataReboot) {
    _dVars.isShutdown = false;
  }

  StartTTSInit();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::BehaviorUpdate()
{
  if(!IsActivated()){
    return;
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::OnBehaviorLeftActivatableScope()
{
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::AlwaysHandleInScope(const RobotToEngineEvent& event)  {
  if(event.GetData().GetTag() == RobotInterface::RobotToEngineTag::startShutdownAnim){
    _dVars.shouldStartPowerOffAnimaiton = true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::StartTTSInit()
{
  auto* greetingAnimAction = new TriggerLiftSafeAnimationAction( AnimationTrigger::GreetAfterLongTime );

  _iConfig.ttsBehavior->SetTextToSay( "Rock, Paper, Or Scissors?" );

  DelegateIfInControl(greetingAnimAction,
    // Callback, will be activated after animation
    // DelegateIfInControl does not hang until completion of action
    [this](){
      // example of defining a new action
      DelegateIfInControl(_iConfig.ttsBehavior.get(), [this]() {
        DelegateIfInControl(_iConfig.hitOrStandPromptBehavior.get(), &BehaviorRockPaperScissors::RockPaperOrScissors);
      });
    }
  );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::RockPaperOrScissors()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();

  int whatdidplayerchoose = 0;

  if (uic.IsUserIntentPending(rockIntent)) {
    uic.DropUserIntent(rockIntent);
    whatdidplayerchoose = 0;
  } else if (uic.IsUserIntentPending(paperIntent)) {
    uic.DropUserIntent(paperIntent);
    whatdidplayerchoose = 1;
  } else if (uic.IsUserIntentPending(scissorsIntent)) {
    uic.DropUserIntent(scissorsIntent);
    whatdidplayerchoose = 2;
  } else {
    // Stand if:
    // 1. We received a valid playerStandIntent or imperative_negative
    if(uic.IsUserIntentPending(silenceIntent)) {
      uic.DropUserIntent(silenceIntent);
    }
    whatdidplayerchoose = 3;
  }

  std::string chosenOne = "";

  if (whatdidplayerchoose == 1) {
    chosenOne = "rock";
  } else if (whatdidplayerchoose == 2) {
    chosenOne = "paper";
  } else if (whatdidplayerchoose == 3) {
    chosenOne = "scissors";
  }

  std::string ttstring = "";
  if (whatdidplayerchoose != 3) {
    std::string ttstring = "Player chose " + chosenOne;
  } else {
    _iConfig.ttsBehavior->SetTextToSay( "Player did not chose, Vector wins" );
  }

  _iConfig.ttsBehavior->SetTextToSay( ttstring );

  DelegateIfInControl(_iConfig.ttsBehavior.get(), [this](){CancelSelf();});
}

}
}
