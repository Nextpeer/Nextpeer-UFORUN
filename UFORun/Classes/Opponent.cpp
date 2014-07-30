//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "Opponent.h"
#include "ViewPort.h"
#include "OpponentUpdate.h"
#include "GameClock.h"
#include "SpriteUpdateInterpolator.h"

// When will this opponent should be considered as timed out?
#define OPPONENT_TIMEOUT_VAL_SECONDS 4

#define IGNORE_SCREEN_DISTANCE_DELTA_THRESHOLD 4.0f
#define ERROR_SCREEN_DISTANCE_DELTA_THRESHOLD 100.0f

Opponent::Opponent(GamePlayerProfile *profile, b2World* world, PlayerData *data) :
_isReadyToStart(false),
_isHurt(false),
_powerUpState(kOpponentPowerUpStateNone),
_lastUpdateTime(0),
_state(kOpponentUnknown),

Player(profile, world, data) {
    _speedX = PLAYER_DEFAULT_LINEAR_SPEED_X;
    showReadyAnimation();
    _state = kOpponentReady;
    
    _interpolator = new SpriteUpdateInterpolator(this, IGNORE_SCREEN_DISTANCE_DELTA_THRESHOLD, ERROR_SCREEN_DISTANCE_DELTA_THRESHOLD);
    _interpolator->retain();
}

Opponent::~Opponent() {
    CC_SAFE_RELEASE_NULL(_interpolator);
}

Opponent* Opponent::create(b2World* world, PlayerData *data)
{
    GamePlayerProfile *profile = GamePlayerProfileFactory::createProfileByTypeOrNull(data->getProfileType());
    
    Opponent *opponent = new Opponent(profile, world, data);
    if (opponent != NULL) {
		opponent->autorelease();
        return opponent;
	}
	CC_SAFE_DELETE(opponent);
	return NULL;
}

void Opponent::update (float dt)
{
    // In case the opponent suppose to stop, reduce the velocity. Otherwise the velocity will be adjusted from the network events.
    if (_state == kOpponentStopped) {
        b2Body* playerBody = getB2Body();
        playerBody->SetLinearVelocity(b2Vec2(0,0));
    }
}

void Opponent::applyOpponentUpdate(OpponentUpdate *update) {
    // Persist the update time (for timed-out checks)
    cc_timeval timeval;
    CCTime::gettimeofdayCocos2d(&timeval, NULL);
    _lastUpdateTime = timeval.tv_sec;
    
    // Update the opponent with the network update message
    updateOpponentFromNetworkUpdate(update);
}

void Opponent::updateOpponentFromNetworkUpdate(OpponentUpdate* update) {
    
    applyHurtUpdate(update);
    
    // Apply power up update
    if (_powerUpState != update->getPowerUpState()) {
        _powerUpState = update->getPowerUpState();
        switch (_powerUpState) {
            case kOpponentPowerUpStateShield:
                showPlayerEffectAnimation(kPlayerEffectAnimationShield);
                break;
                
            case kOpponentPowerUpStateSpeedBoost:
                showPlayerEffectAnimation(kPlayerEffectAnimationSpeedBoost);
                break;
            default:
                stopPlayerEffectAnimation();
                break;
        }
    }
    
    if (_state != update->getState()) {
        _state = update->getState();
        
        showAnimationByState();
    }
    
    // Continue updating as long as the updates matter
    if (_state != kOpponentStopped) {
        this->updateOpponentPositionFromNetworkUpdate(update);
    }
}

void Opponent::showAnimationByState() {
    switch (_state) {
        case kOpponentReady:
            showReadyAnimation();
            break;
        case kOpponentRunning:
            showRunAnimation();
            break;
        case kOpponentJumping:
            showJumpAnimation();
            break;
        case kOpponentStopped:
            showStoppedAnimation();
            break;
        default:
            break;
    }
}

void Opponent::updateOpponentPositionFromNetworkUpdate(OpponentUpdate* update) {
    
    // Calculate the estimated temporal delay between this opponent and our own internal game clock (used in our interpolation calculations)
    float temporalDelay = GameClock::getInstance()->getClock() - update->getTimeStamp() + getClockOffset();
    b2Vec2 networkUpdateVelocity = b2Vec2(update->getLinearVelocityX(), update->getLinearVelocityY());
    b2Vec2 worldPosition = b2Vec2(update->getWorldPositionX(), update->getWorldPositionY());
    _interpolator->updateSpritePositionFromNetworkUpdate(temporalDelay, worldPosition, networkUpdateVelocity, update->getIsStuck());
}

void Opponent::applyHurtUpdate(OpponentUpdate* update) {
    
    // Apply hurt update (enable or disable)
    if (_isHurt != update->getIsHurt()) {
        _isHurt = update->getIsHurt();
        if (_isHurt) {
            showPlayerEffectAnimation(kPlayerEffectAnimationHurt);
        }
        else {
            stopPlayerEffectAnimation();
            
            // Restore the animation of the player according to the inner state (this will make sure the opponent present the right state)
            showAnimationByState();
        }
    }
}

bool Opponent::hasTimedOut()
{
    // The class persists the last update time, if the time is bigger than the constant value it will be conisdered as timed out.
    if (_lastUpdateTime == 0) return false;
    
    cc_timeval timeval;
    CCTime::gettimeofdayCocos2d(&timeval, NULL);
    
    return (timeval.tv_sec - _lastUpdateTime > OPPONENT_TIMEOUT_VAL_SECONDS);
}

void Opponent::simulateHitByFireBallIfRequired() {}
