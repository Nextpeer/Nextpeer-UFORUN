//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__Hero__
#define __UFORun__Hero__

#include "Player.h"
#include "PlayerSpriteProfile.h"

enum HeroBody_TAG
{
	HERO_FOOT 	= 100
};

// Ready -> Running -> FinishLine -> Reached end position (Stopped)
typedef enum
{
    kHeroReady = 0,
    kHeroRunning,
    kHeroPassedFinishLine,
    kHeroStopped
    
} HeroState;

typedef enum
{
    kHeroPowerUpStateNone = 0,
    kHeroPowerUpStateSpeedBoost,
    kHeroPowerUpStateShield
    
} HeroPowerUpState;

/**
 The Hero class. Responsible for hero logic, from movement to finish line.
 The class will use the Box2D physics movement to move the hero.
 */
class Hero : public Player
{
private:
    HeroState _state;
    bool _canJump;
    bool _isStuckOnScreen;
    float _heroLastKnownScreenXPosition;
    float _speedX;
    bool _isHurt;
    HeroPowerUpState _powerUpState;
    
    void addCurrentUserMarker();
    void addFootSensor();
    void move();
    void powerUpUsageFinished();
    void hurtAnimationFinished();
    
protected:
	Hero(GamePlayerProfile *profile, b2World* world, PlayerData *data);
    
public:
    static Hero* create(b2World* world);
    
	virtual void update (float dt);
    
    bool getIsJumping() const;
    
    void startRace();
    void jump();
    
    void passedFinishLine();
    
    void useSpeedBoostPowerUp();
    void useShieldPowerUp();
    void hitByFireBall();
    
    // Read more here http://www.iforce2d.net/b2dtut/jumpability
	void updateJumpability(bool canJump);
    
    float getSpeedX() const{
    	return _speedX;
	}
    
    HeroState getHeroState() const{
    	return _state;
	}
    
    bool isStuck() const{
    	return _isStuckOnScreen;
	}
    
    bool isHurt() const{
    	return _isHurt;
	}
    
    HeroPowerUpState getHeroPowerUpState() const{
    	return _powerUpState;
	}
};

#endif /* defined(__UFORun__Hero__) */
