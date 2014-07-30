//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "OpponentFireBall.h"
#include "OpponentFireBallUpdateMessage.h"
#include "ViewPort.h"
#include "Opponent.h"
#include "GameClock.h"
#include "SpriteUpdateInterpolator.h"

#define OPPONENT_FIRE_BALL_TIMEOUT_VAL_SECONDS 4
#define IGNORE_SCREEN_DISTANCE_DELTA_THRESHOLD 2.0f
#define ERROR_SCREEN_DISTANCE_DELTA_THRESHOLD 50.0f

OpponentFireBall::OpponentFireBall(uint32_t originPowerUpId, Opponent *fromOpponent, b2World* world) : FireBall(originPowerUpId, world) {
    
    _fromOpponent = fromOpponent;
    _lastUpdateTime = 0.0f;
    _fireBallElapsedCounter = 0.0f;
    _lastScreenPosition = CCPointZero;
    
    _interpolator = new SpriteUpdateInterpolator(this, IGNORE_SCREEN_DISTANCE_DELTA_THRESHOLD, ERROR_SCREEN_DISTANCE_DELTA_THRESHOLD);
    _interpolator->retain();
}

OpponentFireBall::~OpponentFireBall() {
    CC_SAFE_RELEASE_NULL(_interpolator);
}

OpponentFireBall* OpponentFireBall::create(uint32_t originPowerUpId, Opponent *fromOpponent, b2World* world){
    
    OpponentFireBall *fireBall = new OpponentFireBall(originPowerUpId, fromOpponent, world);
    
	if (fireBall != NULL) {
		fireBall->autorelease();
        return fireBall;
	}
	CC_SAFE_DELETE(fireBall);
	return NULL;
}

void OpponentFireBall::update (float dt) {
    
    if (getFireBallState() == kFireBallMoving) {
        _fireBallElapsedCounter += dt;
        
        // Fire ball missed the target
        if (_fireBallElapsedCounter >= FIRE_BALL_TIME_LENGTH_SECONDS) {
            setFireBallState(kFireBallStopped);
        }
        // Fire ball hit something (as it went back on the X)
        else if (_lastScreenPosition.x > getPositionX()) {
            setFireBallState(kFireBallStopped);
        }
        // Fire ball hit something (as it went back on different Y)
        else if (_lastScreenPosition.y != getPositionY()) {
            setFireBallState(kFireBallStopped);
        }
    }
    
    // If the fire ball stopped moving? then stop it from moving and hide it as well.
    if (getFireBallState() == kFireBallStopped) {
        b2Body* fireBallBody = getB2Body();
        fireBallBody->SetLinearVelocity(b2Vec2(0,0));
        setOpacity(0);
    }
    
    _lastScreenPosition = getPosition();
}

void OpponentFireBall::applyOpponentFireBallUpdate(OpponentFireBallUpdateMessage *update) {
    
    // Persist the update time (for timed-out checks)
    cc_timeval timeval;
    CCTime::gettimeofdayCocos2d(&timeval, NULL);
    _lastUpdateTime = timeval.tv_sec;
    
    // Update the opponent with the network update message
    updateOpponentFireBallPositionFromNetworkUpdate(update);
}

void OpponentFireBall::updateOpponentFireBallPositionFromNetworkUpdate(OpponentFireBallUpdateMessage *update) {
    
    if (_fromOpponent == NULL) return;
    
    // Calculate the estimated temporal delay between this opponent fire ball and our own internal game clock (used in our interpolation calculations)
    // Add the opponent game clock offset (in case it was a recording, there might be a difference)
    float temporalDelay = GameClock::getInstance()->getClock() - update->getTimeStamp() + _fromOpponent->getClockOffset();
    
    b2Vec2 fireBallVelocity = b2Vec2(FIRE_BALL_SPEED, 0);
    b2Vec2 worldPosition = b2Vec2(update->getWorldPositionX(), update->getWorldPositionY());
    _interpolator->updateSpritePositionFromNetworkUpdate(temporalDelay, worldPosition, fireBallVelocity, false);
    
    // If the fire ball is not moving yet, set it to moving and apply the velocity
    if (getFireBallState() == kFireBallPrepareToFire) {
        // The last known screen position in this case is the one we set at first.
        _lastScreenPosition = getPosition();
        // fireBallBody->SetLinearVelocity(fireBallVelocity);
        setFireBallState(kFireBallMoving);
        return;
    }
}

bool OpponentFireBall::hasTimedOut()
{
    if (_lastUpdateTime == 0) return false;
    
    cc_timeval timeval;
    CCTime::gettimeofdayCocos2d(&timeval, NULL);
    
    return (timeval.tv_sec - _lastUpdateTime > OPPONENT_FIRE_BALL_TIMEOUT_VAL_SECONDS);
}

string OpponentFireBall::getFromOpponentId() {
    if (_fromOpponent ==  NULL) return NULL;
    
    return (_fromOpponent->getPlayerData()->getPlayerId());
}

void OpponentFireBall::markAsStopped() {
    setFireBallState(kFireBallStopped);
}

bool OpponentFireBall::canBeRemoved()
{
    if (_lastUpdateTime == 0) return false;
    
    cc_timeval timeval;
    CCTime::gettimeofdayCocos2d(&timeval, NULL);
    
    return (timeval.tv_sec - _lastUpdateTime > FIRE_BALL_TIME_LENGTH_SECONDS);
}
