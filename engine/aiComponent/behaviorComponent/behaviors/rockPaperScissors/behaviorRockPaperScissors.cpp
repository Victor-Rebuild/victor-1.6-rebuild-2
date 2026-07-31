/**
 * File: BehaviorRockPaperScissors.cpp
 *
 * Author: Emily Modder
 * Created: 2026-06-15
 *
 * Description: Rock paper scissors, playable on Vector
 *
 * Copyright: Victor-Rebuild, 2026
 *
 **/


#include "engine/aiComponent/behaviorComponent/behaviors/rockPaperScissors/behaviorRockPaperScissors.h"

#include "aiComponent/behaviorComponent/behaviorContainer.h"
#include "cannedAnimLib/cannedAnims/cannedAnimationContainer.h"
#include "coretech/common/engine/utils/timer.h"
#include "engine/actions/animActions.h"
#include "engine/aiComponent/behaviorComponent/behaviorContainer.h"
#include "engine/aiComponent/behaviorComponent/behaviorExternalInterface/beiRobotInfo.h"
#include "engine/aiComponent/behaviorComponent/userIntentComponent.h"
#include "engine/aiComponent/behaviorComponent/userIntents.h"
#include "engine/components/localeComponent.h"
#include "engine/externalInterface/externalInterface.h"

namespace Anki {
namespace Vector {

namespace{
  static const UserIntentTag scissorsIntent = USER_INTENT(play_rockPaperScissorsScissors);
  static const UserIntentTag paperIntent = USER_INTENT(play_rockPaperScissorsPaper);
  static const UserIntentTag rockIntent = USER_INTENT(play_rockPaperScissorsRock);
  static const UserIntentTag silenceIntent = USER_INTENT(silence);
  static const UserIntentTag affirmativeIntent = USER_INTENT(imperative_affirmative);
  static const UserIntentTag negativeIntent = USER_INTENT(imperative_negative);
  static const UserIntentTag playAgainIntent = USER_INTENT(blackjack_playagain);
  constexpr const char * kRockPaperScissorsRock = "RockPaperScissors.Rock";
  constexpr const char * kRockPaperScissorsPaper = "RockPaperScissors.Paper";
  constexpr const char * kRockPaperScissorsScissors = "RockPaperScissors.Scissors";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::InstanceConfig::InstanceConfig(const Json::Value& config)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::DynamicVariables::DynamicVariables()
: whatdidplayerchoose()
, howManyBadRequests()
, winLoseTie()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorRockPaperScissors::BehaviorRockPaperScissors(const Json::Value& config)
: ICozmoBehavior(config)
, _iConfig(config)
{
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

  BC.FindBehaviorByIDAndDowncast( BEHAVIOR_ID(RockPaperScissorsChooseMovePrompt),
                                  BEHAVIOR_CLASS(PromptUserForVoiceCommand),
                                  _iConfig.rockPaperScissorsPromptBehavior );

  BC.FindBehaviorByIDAndDowncast( BEHAVIOR_ID(BlackJackRequestToPlayAgain),
                                  BEHAVIOR_CLASS(PromptUserForVoiceCommand),
                                  _iConfig.playAgainPromptBehavior );

  BC.FindBehaviorByIDAndDowncast( BEHAVIOR_ID(KnowledgeGraphTTS),
                                  BEHAVIOR_CLASS(TextToSpeechLoop),
                                  _iConfig.rockPaperScissorsVectorResponseBehavior );
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
  delegates.insert( _iConfig.rockPaperScissorsPromptBehavior.get() );
  delegates.insert(_iConfig.playAgainPromptBehavior.get());
  delegates.insert( _iConfig.rockPaperScissorsVectorResponseBehavior.get() );
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
  _dVars = DynamicVariables();

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
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::StartTTSInit()
{
  const auto & localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();
  _iConfig.ttsBehavior->SetTextToSay( localeComponent.GetString("BehaviorRockPaperScissors.RockPaperOrScissors") );

  DelegateIfInControl(_iConfig.ttsBehavior.get(), [this]() {
    DelegateIfInControl(_iConfig.rockPaperScissorsPromptBehavior.get(), &BehaviorRockPaperScissors::RockPaperOrScissors);
  });
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::RockPaperOrScissors()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  const auto & localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();
  std::string chosenOne = "";
  std::stringstream ss;

  if (uic.IsUserIntentPending(rockIntent)) {
    uic.DropUserIntent(rockIntent);
    _dVars.whatdidplayerchoose = 0;
  } else if (uic.IsUserIntentPending(paperIntent)) {
    uic.DropUserIntent(paperIntent);
    _dVars.whatdidplayerchoose = 1;
  } else if (uic.IsUserIntentPending(scissorsIntent)) {
    uic.DropUserIntent(scissorsIntent);
    _dVars.whatdidplayerchoose = 2;
  } else {
    // Stand if:
    // 1. We received a valid playerStandIntent or imperative_negative
    if(uic.IsUserIntentPending(silenceIntent)) {
      uic.DropUserIntent(silenceIntent);
    }
    _dVars.whatdidplayerchoose = 3;
  }

  const std::string choiceStringLocalized[] = {
    localeComponent.GetString(kRockPaperScissorsRock),
    localeComponent.GetString(kRockPaperScissorsPaper),
    localeComponent.GetString(kRockPaperScissorsScissors)
  };

  if (_dVars.whatdidplayerchoose != 3) {
    chosenOne = choiceStringLocalized[_dVars.whatdidplayerchoose];
  } else if (_dVars.whatdidplayerchoose == 3 && _dVars.howManyBadRequests <= 3) {
    _dVars.howManyBadRequests = _dVars.howManyBadRequests + 1;
    DelegateIfInControl(_iConfig.rockPaperScissorsPromptBehavior.get(), &BehaviorRockPaperScissors::RockPaperOrScissors);
  } else {
    _iConfig.ttsBehavior->SetTextToSay( localeComponent.GetString("RockPaperScissors.NoChoice") );
    _dVars.winLoseTie = 2;
    DelegateIfInControl(_iConfig.ttsBehavior.get(), &BehaviorRockPaperScissors::PlayWinLoseTieAnim);
  }

  ss << localeComponent.GetString("RockPaperScissors.PlayerChose") + " " + chosenOne + ", " + localeComponent.GetString("RockPaperScissors.VectorTurn");

  LOG_WARNING("BehaviorRockPaperScissors", "Final string is: %s", ss.str().c_str());

  _iConfig.ttsBehavior->SetTextToSay( ss.str() );

  DelegateIfInControl(_iConfig.ttsBehavior.get(), &BehaviorRockPaperScissors::RockPaperOrScissorsVectorSearch);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::RockPaperOrScissorsVectorSearch()
{
  CompoundActionSequential *messageAnimation = new CompoundActionSequential();
  messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::KnowledgeGraphGetIn), true);
  messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::KnowledgeGraphSearchingGetIn), true);
  messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::KnowledgeGraphSearching), true);
  messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::KnowledgeGraphSearchingGetOutSuccess), true);
  DelegateIfInControl(messageAnimation, &BehaviorRockPaperScissors::RockPaperOrScissorsVector);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::RockPaperOrScissorsVector()
{

  srand(BaseStationTimer::getInstance()->GetCurrentTimeInSeconds());
  int whatdidvectorchoose = rand() % 3;
  const auto & localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();

  std::string chosenOne = "";
  std::string finalString = "";
  std::string winLoosePush = "";

  const std::string vectorWins = localeComponent.GetString("RockPaperScissors.VectorWins");
  const std::string vectorLost = localeComponent.GetString("RockPaperScissors.VectorLoss");
  const std::string vectorTied = localeComponent.GetString("RockPaperScissors.VectorTied");

  const std::string choiceStringLocalized[] = {
    localeComponent.GetString(kRockPaperScissorsRock),
    localeComponent.GetString(kRockPaperScissorsPaper),
    localeComponent.GetString(kRockPaperScissorsScissors)
  };

  chosenOne = choiceStringLocalized[whatdidvectorchoose];

  if (whatdidvectorchoose == _dVars.whatdidplayerchoose)
  {
    winLoosePush = vectorTied;
  } else if ((whatdidvectorchoose == 0 && _dVars.whatdidplayerchoose == 1) ||
             (whatdidvectorchoose == 1 && _dVars.whatdidplayerchoose == 2) ||
             (whatdidvectorchoose == 2 && _dVars.whatdidplayerchoose == 0))
  {
    winLoosePush = vectorLost;
  } else if ((whatdidvectorchoose == 0 && _dVars.whatdidplayerchoose == 2) ||
             (whatdidvectorchoose == 1 && _dVars.whatdidplayerchoose == 0) ||
             (whatdidvectorchoose == 2 && _dVars.whatdidplayerchoose == 1))
  {
    winLoosePush = vectorWins;
  }

  if (winLoosePush == vectorLost) {
    _dVars.winLoseTie = 0;
  } else if (winLoosePush == vectorTied) {
    _dVars.winLoseTie = 1;
  } else if (winLoosePush == vectorWins) {
    _dVars.winLoseTie = 2;
  }

  finalString = localeComponent.GetString("RockPaperScissors.VectorChose") + " " + chosenOne + "." + winLoosePush;

  _iConfig.rockPaperScissorsVectorResponseBehavior->SetTextToSay( finalString );
  DelegateIfInControl(_iConfig.rockPaperScissorsVectorResponseBehavior.get(), &BehaviorRockPaperScissors::PlayWinLoseTieAnim);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::PlayWinLoseTieAnim()
{
  CompoundActionSequential *messageAnimation = new CompoundActionSequential();
  if (_dVars.winLoseTie == 0) {
    messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::BlackJack_VictorLose), true);
  } else if (_dVars.winLoseTie == 1) {
    messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::BlackJack_VictorPush), true);
  } else if (_dVars.winLoseTie == 2) {
    messageAnimation->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::BlackJack_VictorWin), true);
  }
  DelegateIfInControl(messageAnimation, &BehaviorRockPaperScissors::TransitionToPlayAgainPrompt);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::TransitionToPlayAgainPrompt()
{
  if(_iConfig.playAgainPromptBehavior->WantsToBeActivated()){
    DelegateIfInControl(_iConfig.playAgainPromptBehavior.get(), &BehaviorRockPaperScissors::TransitionToPlayAgain);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorRockPaperScissors::TransitionToPlayAgain()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();

  if(uic.IsUserIntentPending(playAgainIntent)){
    uic.DropUserIntent(playAgainIntent);
    OnBehaviorActivated();
  } else if(uic.IsUserIntentPending(affirmativeIntent)){
    uic.DropUserIntent(affirmativeIntent);
    OnBehaviorActivated();
  } else {
    if (uic.IsUserIntentPending(negativeIntent)){
      uic.DropUserIntent(negativeIntent);
    } else if(uic.IsUserIntentPending(silenceIntent)) {
      uic.DropUserIntent(silenceIntent);
    }
    DelegateIfInControl(new TriggerAnimationAction(AnimationTrigger::BlackJack_Quit), [this](){CancelSelf();});
  }
}

}
}
