/**
 * File: BehaviorSnakeGame.cpp
 *
 * Author: ross
 * Created: 2018-02-27
 *
 * Description: victor plays the game snake. his solver is greedy, so it eventually fails
 *
 * Copyright: Anki, Inc. 2018
 *
 **/


#include "engine/aiComponent/behaviorComponent/behaviorContainer.h"
#include "engine/aiComponent/behaviorComponent/behaviors/victor/behaviorSnakeGame.h"
#include "engine/aiComponent/behaviorComponent/behaviors/victor/snakeGame.h"
#include "engine/aiComponent/behaviorComponent/behaviors/victor/snakeGameSolver.h"
#include "engine/aiComponent/behaviorComponent/behaviorExternalInterface/beiRobotInfo.h"
#include "engine/aiComponent/behaviorComponent/userIntentComponent.h"
#include "engine/aiComponent/behaviorComponent/userIntents.h"

#include "coretech/common/engine/colorRGBA.h"
#include "coretech/common/engine/utils/timer.h"
#include "coretech/vision/engine/image.h"

#include "engine/actions/animActions.h"
#include "engine/actions/basicActions.h"

#include "engine/audio/engineRobotAudioClient.h"

#include "engine/components/animationComponent.h"
#include "engine/components/localeComponent.h"
#include "engine/components/rebuildConfig.h"

#include "clad/audio/audioEventTypes.h"

namespace Anki {
namespace Vector {

using AMD_GE_GE = AudioMetaData::GameEvent::GenericEvent;
using AMD_GOT = AudioMetaData::GameObjectType;

namespace {
  unsigned int kTicksPerGameUpdate = 1;
  unsigned int kScalingFactor = 4; // must be power of 2
  const char * kVictorScore = "BehaviorSnakeGame.VictorScore";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorSnakeGame::BehaviorSnakeGame(const Json::Value& config)
 : ICozmoBehavior(config)
{
  // TODO: read config into _iConfig
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BehaviorSnakeGame::~BehaviorSnakeGame()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BehaviorSnakeGame::WantsToBeActivatedBehavior() const
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  srand(BaseStationTimer::getInstance()->GetCurrentTimeInSeconds());
  return (rand() % 10 <= 4) || uic.IsUserIntentPending(USER_INTENT(snake_victor_score));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::GetBehaviorOperationModifiers(BehaviorOperationModifiers& modifiers) const
{
  modifiers.wantsToBeActivatedWhenOnCharger = true;
  modifiers.wantsToBeActivatedWhenOffTreads = false;
  modifiers.behaviorAlwaysDelegates = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::InitBehavior()
{
  auto& BC = GetBEI().GetBehaviorContainer();
  BC.FindBehaviorByIDAndDowncast( BEHAVIOR_ID(DefaultTextToSpeechLoop),
                                BEHAVIOR_CLASS(TextToSpeechLoop),
                                _iConfig.ttsBehavior );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::GetAllDelegates(std::set<IBehavior*>& delegates) const
{
  delegates.insert(_iConfig.ttsBehavior.get());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::OnBehaviorActivated()
{
  // reset dynamic variables
  _dVars = DynamicVariables();

  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  UserIntentPtr intentDataVictorScore = uic.GetUserIntentIfActive(USER_INTENT(snake_victor_score));

  if (intentDataVictorScore) {
    SayScore();
    return;
  }

  // Play getin
  auto* action = new TriggerLiftSafeAnimationAction( AnimationTrigger::BlackJack_GetIn );
  DelegateIfInControl(action, [&](ActionResult result) {
    auto& rng = GetBEI().GetRNG();

    // move head up
    DelegateIfInControl(new MoveHeadToAngleAction( M_PI/4 ));

    unsigned int initLength = 10;

    unsigned int snakeWidth = (FACE_DISPLAY_WIDTH - 2*kScalingFactor)/kScalingFactor;
    unsigned int snakeHeight = (FACE_DISPLAY_HEIGHT - 2*kScalingFactor)/kScalingFactor;

    _dVars.image.reset( new Vision::Image( FACE_DISPLAY_HEIGHT, FACE_DISPLAY_WIDTH, NamedColors::BLACK ) );
    _dVars.game.reset( new SnakeGame( snakeWidth, snakeHeight, initLength, rng ) );
    const float probMistakePerLength = 0.001f;
    _dVars.solver.reset( new SnakeGameSolver( *_dVars.game.get(), 0.0f, probMistakePerLength, 0.0f, rng ) );
  });
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::OnBehaviorDeactivated()
{
  if (_dVars.points > RebuildToggles::GetInt("snakeHighScoreVector")) {
    RebuildToggles::SetInt(nullptr, "snakeHighScoreVector", _dVars.points);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::BehaviorUpdate()
{
  if( !IsActivated() ) {
    return;
  }
  if( _dVars.image == nullptr ) {
    // not finished with head motion yet
    return;
  }
  if( _dVars.lost ) {
    return;
  }

  auto& image = *_dVars.image.get();

  ++_dVars.gameTicks;
  if( _dVars.gameTicks >= kTicksPerGameUpdate ) {
    _dVars.gameTicks = 0;

    // update game and solver
    _dVars.game->Update();

    if( _dVars.game->GameOver() ) {
      _dVars.lost = true;
      CancelDelegates(false);
      // lost. play an animation and end
      CompoundActionSequential *newAction = new CompoundActionSequential();
      newAction->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::BlackJack_Swipe), true);
      newAction->AddAction(new TriggerLiftSafeAnimationAction(AnimationTrigger::Feedback_ShutUp), true);
      DelegateIfInControl(newAction, [this](ActionResult result) {
        CancelSelf();
      });
      return;
    }

    auto oldDirection = _dVars.game->GetDirection();
    _dVars.solver->ChooseAndApplyMove();
    auto newDirection = _dVars.game->GetDirection();

    if( oldDirection != newDirection ) {
      AnimateDirection( static_cast<uint8_t>(newDirection) );
    }

    // clear face
    image = Vision::Image( FACE_DISPLAY_HEIGHT, FACE_DISPLAY_WIDTH, NamedColors::BLACK );

    // draw face
    DrawGame( image );

    // if (_dVars.prevPoints != _dVars.points) {
    //   _dVars.prevPoints = _dVars.points;
    //   LOG_WARNING("BehaviorSnakeGame", "Current points %i", _dVars.points);
    // }

    GetBEI().GetAnimationComponent().DisplayFaceImage( image, 0.0f, true );
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::SayScore()
{
  UserIntentComponent& uic = GetBehaviorComp<UserIntentComponent>();
  const auto & localeComponent = GetBEI().GetRobotInfo().GetLocaleComponent();
  static const UserIntentTag victorScoreIntent = USER_INTENT(snake_victor_score);
  uic.DropUserIntent(victorScoreIntent);
  _iConfig.ttsBehavior->SetTextToSay(localeComponent.GetString(kVictorScore, std::to_string(RebuildToggles::GetInt("snakeHighScoreVector"))));
  DelegateIfInControl(_iConfig.ttsBehavior.get(), [this](){CancelSelf();});
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BehaviorSnakeGame::DrawGame( Vision::Image& image ) const
{
  if( !ANKI_VERIFY(_dVars.game != nullptr,
                   "",
                   "") )
  {
    return;
  }

  const auto& game = *_dVars.game.get();

  int height = image.GetNumRows();
  int width = image.GetNumCols();
  if( !ANKI_VERIFY( (width == kScalingFactor*(2+game.GetWidth())) && (height == kScalingFactor*(2+game.GetHeight())) ,
                    "",
                    "" ) )
  {
    return;
  }


  auto* pImg = image.GetRawDataPointer();

  auto setImg = [pImg,width](int i, int j, u8 value) {
    *(pImg + (j*width) + i) = value;
  };



  for( int j=0; j<height; ++j ) {
    for( int i=0; i<width; ++i ) {
      unsigned int snakeI = (i - kScalingFactor) / kScalingFactor;
      unsigned int snakeJ = (j - kScalingFactor) / kScalingFactor;
      if( i<kScalingFactor || j<kScalingFactor || i>=width-kScalingFactor || j>=height-kScalingFactor ) {
        setImg(i,j, 255);
        continue;
      }
      if( game.GetSnake().IsSnakeAt(snakeI, snakeJ) ) {
        setImg(i,j, 255);
      } else if( game.GetFood().x == snakeI && game.GetFood().y == snakeJ ) {
        setImg(i,j, 255);
      }
      if (game.GetScore() != _dVars.points) {
        auto& dVars = const_cast<DynamicVariables&>(_dVars);
        dVars.points = game.GetScore();
        GetBEI().GetRobotAudioClient().PostEvent(AMD_GE_GE::Play__Robot_Vic_Sfx__Blackjack_Deal, AMD_GOT::Behavior);
      }
    }
  }
}

void BehaviorSnakeGame::AnimateDirection( uint8_t directionInt )
{
  auto direction = static_cast<SnakeGame::Direction>(directionInt);
  if( (direction == SnakeGame::Direction::LEFT)
      || (direction == SnakeGame::Direction::RIGHT) )
  {
    float degrees = 0.0f;
    // note: this is inverted... screen right is victor left
    if( direction == SnakeGame::Direction::LEFT ) {
      degrees = -10.f;
    } else {
      degrees = 10.f;
    }
    auto turnAction = new TurnInPlaceAction( DEG_TO_RAD(degrees), false );
    turnAction->SetAccel(MAX_BODY_ROTATION_ACCEL_RAD_PER_SEC2 / 2);
    turnAction->SetMaxSpeed(MAX_BODY_ROTATION_SPEED_RAD_PER_SEC);
    CancelDelegates();
    DelegateIfInControl(turnAction);
  } else {
    IAction* liftAction;
    if( direction == SnakeGame::Direction::DOWN ) {
      liftAction = new MoveLiftToHeightAction(MoveLiftToHeightAction::Preset::JUST_ABOVE_PROX); // not working
    } else {
      liftAction = new MoveLiftToHeightAction(MoveLiftToHeightAction::Preset::LOW_DOCK);
    }
    CancelDelegates();
    DelegateIfInControl(liftAction);
  }
}

}
}
