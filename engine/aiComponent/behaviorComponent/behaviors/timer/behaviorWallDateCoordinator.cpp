/**
* File: BehaviorWallDateCoordinator.cpp
*
* Author: Kevin M. Karol
* Created: 2018-06-15
*
* Description: Manage the designed response to a user request for the wall time
*
* Copyright: Anki, Inc. 2018
*
**/


#include "engine/aiComponent/behaviorComponent/behaviors/timer/behaviorWallDateCoordinator.h"

#include "clad/audio/audioSwitchTypes.h"
#include "components/textToSpeech/textToSpeechCoordinator.h"
#include "engine/aiComponent/behaviorComponent/behaviorContainer.h"
#include "engine/aiComponent/behaviorComponent/behaviors/timer/behaviorDisplayWallDate.h"
#include "engine/actions/animActions.h"
#include "engine/components/settingsManager.h"
#include "engine/faceWorld.h"
#include "osState/wallTime.h"

#include <iomanip>

namespace Anki {
namespace Vector {
  

namespace{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorWallDateCoordinator::InstanceConfig::InstanceConfig()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorWallDateCoordinator::DynamicVariables::DynamicVariables()
{
  utteranceID = kInvalidUtteranceID;
  utteranceState = UtteranceState::Generating;
  isShowingTime = false;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorWallDateCoordinator::BehaviorWallDateCoordinator(const Json::Value& config)
: ICozmoBehavior(config)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorWallDateCoordinator::~BehaviorWallDateCoordinator()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::GetAllDelegates(std::set<IBehavior*>& delegates) const
{
  delegates.insert(_iConfig.iCantDoThatBehavior.get());
  delegates.insert(_iConfig.showWallDate.get());
  delegates.insert(_iConfig.lookAtFaceInFront.get());
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const
{
  modifiers.behaviorAlwaysDelegates = true;
  modifiers.wantsToBeActivatedWhenOffTreads = true;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::GetBehaviorJsonKeys(std::set<const char*>& expectedKeys) const 
{
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BehaviorWallDateCoordinator::WantsToBeActivatedBehavior() const
{
  return true;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::InitBehavior()
{
  auto& behaviorContainer = GetBEI().GetBehaviorContainer();

  _iConfig.iCantDoThatBehavior = behaviorContainer.FindBehaviorByID(BEHAVIOR_ID(SingletonICantDoThat));
  _iConfig.lookAtFaceInFront   = behaviorContainer.FindBehaviorByID(BEHAVIOR_ID(SingletonFindFaceInFrontWallTime));

  behaviorContainer.FindBehaviorByIDAndDowncast(BEHAVIOR_ID(ShowWallDate), 
                                                BEHAVIOR_CLASS(DisplayWallDate),
                                                _iConfig.showWallDate);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::OnBehaviorActivated()
{
  _dVars = DynamicVariables();

  if(WallTime::getInstance()->GetApproximateLocalTime(_dVars.date)){
    StartTTSGeneration();
    // let's look for a face while we're generating TTS
    TransitionToFindFaceInFront();
  }

  // if we failed to start the "find face" behavior, we need to bail
  if(!IsControlDelegated()){
    TransitionToICantDoThat();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::BehaviorUpdate()
{
  if(!IsActivated()){
    return;
  }

  if (!_dVars.isShowingTime){
    switch (_dVars.utteranceState)
    {
      case UtteranceState::Ready:
      case UtteranceState::Invalid:
        // cancel look for face and immediately show wall clock once we're ready
        // safe to call when nothing is currently delegated
        CancelDelegates(false);
        TransitionToShowWallDate();
        break;

      default:
        break;
    }
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::OnBehaviorDeactivated()
{
  if (kInvalidUtteranceID != _dVars.utteranceID){
    GetBEI().GetTextToSpeechCoordinator().CancelUtterance(_dVars.utteranceID);
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::TransitionToICantDoThat()
{
  ANKI_VERIFY(_iConfig.iCantDoThatBehavior->WantsToBeActivated(), 
              "BehaviorWallDateCoordinator.TransitionToICantDoThat.BehaviorDoesntWantToBeActivated", "");
  DelegateIfInControl(_iConfig.iCantDoThatBehavior.get());
  // annnnnd we're done (behaviorAlwaysDelegates = false)
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::TransitionToFindFaceInFront()
{
  ANKI_VERIFY(_iConfig.lookAtFaceInFront->WantsToBeActivated(),
              "BehaviorWallDateCoordinator.TransitionToshowWallDate.BehaviorDoesntWantToBeActivated", "");
  DelegateIfInControl(_iConfig.lookAtFaceInFront.get(), [this](){
    DelegateIfInControl(new TriggerLiftSafeAnimationAction(AnimationTrigger::LookAtUserEndearingly));
  });
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::TransitionToShowWallDate()
{
  _dVars.isShowingTime = true;

  auto playUtteranceCallback = [this](){
    // only play TTS if it was generated, else we're fine with just the clock
    if ((kInvalidUtteranceID != _dVars.utteranceID) && (_dVars.utteranceState == UtteranceState::Ready)){
      GetBEI().GetTextToSpeechCoordinator().PlayUtterance(_dVars.utteranceID);
    } else {
      LOG_ERROR("BehaviorWallDateCoordinator", "Attempted to play time TTS but generation failed");
    }
  };

  _iConfig.showWallDate->SetShowClockCallback(playUtteranceCallback);
  _iConfig.showWallDate->SetOverrideDisplayTime(_dVars.date);

  ANKI_VERIFY(_iConfig.showWallDate->WantsToBeActivated(),
              "BehaviorWallDateCoordinator.TransitionToshowWallDate.BehaviorDoesntWantToBeActivated", "");
  DelegateIfInControl(_iConfig.showWallDate.get());
  // annnnnd we're done (behaviorAlwaysDelegates = false)
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorWallDateCoordinator::StartTTSGeneration()
{
  auto textOfTime = GetTTSStringForDate();

  const UtteranceTriggerType triggerType = UtteranceTriggerType::Manual;
  const AudioTtsProcessingStyle style = AudioTtsProcessingStyle::Default_Processed;

  auto callback = [this](const UtteranceState& utteranceState)
  {
    _dVars.utteranceState = utteranceState;
  };

  _dVars.utteranceID = GetBEI().GetTextToSpeechCoordinator().CreateUtterance(textOfTime, triggerType, style,
                                                                             1.0f, callback);

  // if we failed to create the tts, we need to let our behavior know since the callback is NOT called in this case
  if (kInvalidUtteranceID == _dVars.utteranceID){
    _dVars.utteranceState = UtteranceState::Invalid;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string BehaviorWallDateCoordinator::GetTTSStringForDate()
{
  std::stringstream ss;
  struct tm localDate;
  WallTime::getInstance()->GetLocalTime(localDate);

  int currentMonth = localDate.tm_mon + 1;
  int currentDay = localDate.tm_mday;

  std::string monthString;
  std::string dayString;

  if (currentMonth == 1) {
    monthString = "January";
  } else if (currentMonth == 2) {
    monthString = "February";
  } else if (currentMonth == 3) {
    monthString = "March";
  } else if (currentMonth == 4) {
    monthString = "April";
  } else if (currentMonth == 5) {
    monthString = "May";
  } else if (currentMonth == 6) {
    monthString = "June";
  } else if (currentMonth == 7) {
    monthString = "July";
  } else if (currentMonth == 8) {
    monthString = "August";
  } else if (currentMonth == 9) {
    monthString = "September";
  } else if (currentMonth == 10) {
    monthString = "October";
  } else if (currentMonth == 11) {
    monthString = "November";
  } else if (currentMonth == 12) {
    monthString = "December";
  }

  if ((currentDay % 100 >= 11) && (currentDay % 100 <= 13)) {
    dayString = "th";
  } else if (currentDay % 10 == 1) {
    dayString = "st";
  } else if (currentDay % 10 == 2) {
    dayString = "nd";
  } else if (currentDay % 10 == 3) {
    dayString = "rd";
  } else {
    dayString = "th";
  }
    
  ss << monthString + std::to_string(currentDay) + dayString;

  return ss.str();
}



} // namespace Vector
} // namespace Anki
