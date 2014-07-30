//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__OpponentFireBall__
#define __UFORun__OpponentFireBall__

#include "FireBall.h"
#include <string>
using namespace std;

class OpponentFireBallUpdateMessage;
class Opponent;
class SpriteUpdateInterpolator;

/**
 Represents the opponent's fire ball (laser) in the game. The class will render the fire ball based on network updates.
 */
class OpponentFireBall : public FireBall
{
private:
    long _lastUpdateTime;
    float _fireBallElapsedCounter;
    CCPoint _lastScreenPosition;
    Opponent *_fromOpponent; // The opponent that fired this fireball
    SpriteUpdateInterpolator *_interpolator;
    
    void updateOpponentFireBallPositionFromNetworkUpdate(OpponentFireBallUpdateMessage* update);
    
public:
    OpponentFireBall(uint32_t originPowerUpId, Opponent *fromOpponent, b2World* world);
    ~OpponentFireBall();
    
    static OpponentFireBall* create(uint32_t originPowerUpId, Opponent *fromOpponent, b2World* world);
    
    virtual void applyOpponentFireBallUpdate(OpponentFireBallUpdateMessage* update);
    
	virtual void update (float dt);
    
    virtual bool hasTimedOut();
    
    virtual string getFromOpponentId();
    
    // The fire ball has stopped, either by timeout, hit an obstacle or opponent. It is no longer valid.
    void markAsStopped();
    
    // The fire ball can be removed by the system (making sure that the last update time passed the FIRE_BALL_TIME_LENGTH_SECONDS)
    bool canBeRemoved();
};
#endif /* defined(__UFORun__OpponentFireBall__) */
