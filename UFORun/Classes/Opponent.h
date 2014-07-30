//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__Opponent__
#define __UFORun__Opponent__

#include "Player.h"
#include "PlayerSpriteProfile.h"
#include "PlayerData.h"

#include <string>
using namespace std;

class OpponentUpdate;
class SpriteUpdateInterpolator;

typedef enum
{
    kOpponentUnknown = 0,
    kOpponentReady,
    kOpponentRunning,
    kOpponentJumping,
    kOpponentStopped
    
} OpponentState;

typedef enum
{
    kOpponentPowerUpStateNone = 0,
    kOpponentPowerUpStateSpeedBoost,
    kOpponentPowerUpStateShield
    
} OpponentPowerUpState;

/**
 The opponent in the game. Simulated movement, state and action by network updates.
 */
class Opponent : public Player
{
private:
    long _lastUpdateTime;
    OpponentState _state;
    float _speedX;
    bool _isHurt;
    OpponentPowerUpState _powerUpState;
    SpriteUpdateInterpolator *_interpolator;
    
protected:
    
    Opponent(GamePlayerProfile *profile, b2World* world, PlayerData *data);
    ~Opponent();
    
    virtual void updateOpponentFromNetworkUpdate(OpponentUpdate* update);
    void updateOpponentPositionFromNetworkUpdate(OpponentUpdate* update);
    virtual void applyHurtUpdate(OpponentUpdate* update);
    
    void showAnimationByState();
    
    void setSpeedX(float speedX) {
        _speedX = speedX;
    }
    
    float getSpeedX() const{
    	return _speedX;
	}
    
    void setIsHurt(bool isHurt) {
        _isHurt = isHurt;
    }
    
    bool getIsHurt() const {
        return _isHurt;
    }
    
    OpponentState getOpponentState() const {
        return _state;
    }
    
public:
    static Opponent* create(b2World* world, PlayerData *data);
    
	virtual void update (float dt);
    
    // The game clock offset, use by subclass to simulate an offset in the game clock
    virtual float getClockOffset() const {
        return 0;
    }
    
    void applyOpponentUpdate(OpponentUpdate* update);
    virtual bool hasTimedOut();
    virtual void simulateHitByFireBallIfRequired();
    
    OpponentPowerUpState getOpponentPowerUpState() const{
    	return _powerUpState;
	}
    CC_SYNTHESIZE(bool, _isReadyToStart, IsReadyToStart);
};

#endif /* defined(__UFORun__Opponent__) */
