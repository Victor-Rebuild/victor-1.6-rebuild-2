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
#include "engine/aiComponent/behaviorComponent/behaviorExternalInterface/beiRobotInfo.h"
#include "engine/actions/animActions.h"
#include "engine/components/localeComponent.h"
#include "engine/components/settingsManager.h"
#include "engine/faceWorld.h"
#include "osState/wallTime.h"

#include <iomanip>
#include <string>

namespace Anki {
namespace Vector {
  

namespace{
  constexpr const char * kDateJanuary = "BehaviorDate.Month1";
  constexpr const char * kDateFebruary = "BehaviorDate.Month2";
  constexpr const char * kDateMarch = "BehaviorDate.Month3";
  constexpr const char * kDateApril = "BehaviorDate.Month4";
  constexpr const char * kDateMay = "BehaviorDate.Month5";
  constexpr const char * kDateJune = "BehaviorDate.Month6";
  constexpr const char * kDateJuly = "BehaviorDate.Month7";
  constexpr const char * kDateAugust = "BehaviorDate.Month8";
  constexpr const char * kDateSeptember = "BehaviorDate.Month9";
  constexpr const char * kDateOctober = "BehaviorDate.Month10";
  constexpr const char * kDateNovember = "BehaviorDate.Month11";
  constexpr const char * kDateDecember = "BehaviorDate.Month12";

  constexpr const char * kDaySunday = "BehaviorDate.Day1";
  constexpr const char * kDayMonday = "BehaviorDate.Day2";
  constexpr const char * kDayTuesday = "BehaviorDate.Day3";
  constexpr const char * kDayWednesday = "BehaviorDate.Day4";
  constexpr const char * kDayThursday = "BehaviorDate.Day5";
  constexpr const char * kDayFriday = "BehaviorDate.Day6";
  constexpr const char * kDaySaturday = "BehaviorDate.Day7";
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
  int dayWeek = 0;

  std::string buffer = " ";
  std::string monthString;
  std::string dayString;
  std::string dayStringEnd;
  WallTime::getInstance()->GetDayOfWeek(dayWeek);
  const auto & localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();

  dayWeek = dayWeek + 1;

  if (dayWeek == 1) {
    dayString = localeComponent.GetString(kDaySunday);
  } else if (dayWeek == 2) {
    dayString = localeComponent.GetString(kDayMonday);
  } else if (dayWeek == 3) {
    dayString = localeComponent.GetString(kDayTuesday);
  } else if (dayWeek == 4) {
    dayString = localeComponent.GetString(kDayWednesday);
  } else if (dayWeek == 5) {
    dayString = localeComponent.GetString(kDayThursday);
  } else if (dayWeek == 6) {
    dayString = localeComponent.GetString(kDayFriday);
  } else if (dayWeek == 7) {
    dayString = localeComponent.GetString(kDaySaturday);
  }

  if (currentMonth == 1) {
    monthString = localeComponent.GetString(kDateJanuary);
  } else if (currentMonth == 2) {
    monthString = localeComponent.GetString(kDateFebruary);
  } else if (currentMonth == 3) {
    monthString = localeComponent.GetString(kDateMarch);
  } else if (currentMonth == 4) {
    monthString = localeComponent.GetString(kDateApril);
  } else if (currentMonth == 5) {
    monthString = localeComponent.GetString(kDateMay);
  } else if (currentMonth == 6) {
    monthString = localeComponent.GetString(kDateJune);
  } else if (currentMonth == 7) {
    monthString = localeComponent.GetString(kDateJuly);
  } else if (currentMonth == 8) {
    monthString = localeComponent.GetString(kDateAugust);
  } else if (currentMonth == 9) {
    monthString = localeComponent.GetString(kDateSeptember);
  } else if (currentMonth == 10) {
    monthString = localeComponent.GetString(kDateOctober);
  } else if (currentMonth == 11) {
    monthString = localeComponent.GetString(kDateNovember);
  } else if (currentMonth == 12) {
    monthString = localeComponent.GetString(kDateDecember);
  }

  if ((currentDay % 100 >= 11) && (currentDay % 100 <= 13)) {
    dayStringEnd = "th";
  } else if (currentDay % 10 == 1) {
    dayStringEnd = "st";
  } else if (currentDay % 10 == 2) {
    dayStringEnd = "nd";
  } else if (currentDay % 10 == 3) {
    dayStringEnd = "rd";
  } else {
    dayStringEnd = "th";
  }
    
  ss << dayString + buffer + monthString + buffer + std::to_string(currentDay) + dayStringEnd;
  LOG_WARNING("Date", "DateString: %s", ss.str().c_str());

  return ss.str();
}



} // namespace Vector
} // namespace Anki
