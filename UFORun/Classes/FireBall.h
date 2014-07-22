//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__FireBall__
#define __UFORun__FireBall__

#include "cocos2d.h"
USING_NS_CC;
#include "cocos-ext.h"
#include "Box2D.h"

using namespace std;
USING_NS_CC_EXT;

#define FIRE_BALL_TIME_LENGTH_SECONDS 3.0
#define FIRE_BALL_SPEED 10.0

/**
 Abstract class. Defines the fire ball (laser) in the game. Responsible for moving the fire ball on the screen and validate its state.
 */

// PrepareToFire -> Moving ->[time elapsed] -> Stopped [Due to hit, or timeout]
typedef enum
{
    kFireBallPrepareToFire = 0,
    kFireBallMoving,
    kFireBallStopped
    
} FireBallState;

class FireBall : public CCPhysicsSprite
{
private:
    FireBallState _state;
    uint32_t _originPowerUpId; // The power up id that this fire ball came from
    void buildFireBallBody(b2World* world);

protected:
    
    void setFireBallState(FireBallState state) {
        _state = state;
    }
    
public:
    
    FireBall(uint32_t originPowerUpId, b2World* world);
    
    uint32_t getOriginPowerUpId() const{
    	return _originPowerUpId;
	}
    
    virtual FireBallState getFireBallState() const{
    	return _state;
	}
    
	virtual void update (float dt) = 0;
    virtual bool isHit(CCNode* target);
};

#endif /* defined(__UFORun__FireBall__) */
