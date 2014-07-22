//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "HeroFireBall.h"

#define FIRE_BALL_FADE_OUT_ANIMATINO_START_SECONDS FIRE_BALL_TIME_LENGTH_SECONDS - FIRE_BALL_TIME_LENGTH_SECONDS/2
#define FIRE_BALL_FADE_OUT_ANIMATINO_START_LAST FIRE_BALL_TIME_LENGTH_SECONDS - FIRE_BALL_FADE_OUT_ANIMATINO_START_SECONDS

HeroFireBall::HeroFireBall(CCPoint initialFirePosition, uint32_t fromPowerUpId, b2World* world) : FireBall(fromPowerUpId, world) {
    _initialFirePosition = initialFirePosition;
    _fireBallElapsedCounter = 0.0f;
    _lastScreenPosition = CCPointZero;
    _startedFadeOutAnimation = false;
}

HeroFireBall* HeroFireBall::create(CCPoint initialFirePosition, uint32_t fromPowerUpId, b2World* world){
    
    HeroFireBall *fireBall = new HeroFireBall(initialFirePosition, fromPowerUpId, world);
    
	if (fireBall != NULL) {
		fireBall->autorelease();
        return fireBall;
	}
	CC_SAFE_DELETE(fireBall);
	return NULL;
}

void HeroFireBall::update (float dt) {
    // In case that the fire ball has entered the update loop without moving (in prepare to fire state). Make sure to fire it from the initial position
    if (getFireBallState() == kFireBallPrepareToFire) {
        // Fire the fire ball from the initial fire position which passed as a paramater
        _lastScreenPosition = _initialFirePosition;
        setPosition(_initialFirePosition);
        setFireBallState(kFireBallMoving);
    }
    
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
        // Else, if it is time to show the fade out animation
        else if (!_startedFadeOutAnimation && _fireBallElapsedCounter >= FIRE_BALL_FADE_OUT_ANIMATINO_START_SECONDS) {
            // Mark that the action has started already
            _startedFadeOutAnimation = true;
            
            this->runAction(CCFadeOut::create(FIRE_BALL_FADE_OUT_ANIMATINO_START_LAST));
        }
    }
    
    // If the fire ball is still moving? assign the current speed x to its box2D body.
    if (getFireBallState() == kFireBallMoving) {
        b2Body* fireBallBody = getB2Body();
        b2Vec2 currentLinearVel = fireBallBody->GetLinearVelocity();
        currentLinearVel.x = FIRE_BALL_SPEED;
        fireBallBody->SetLinearVelocity(currentLinearVel);
    }
    
    _lastScreenPosition = getPosition();
}
