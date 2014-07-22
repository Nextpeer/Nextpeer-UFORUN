//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__HeroFireBall__
#define __UFORun__HeroFireBall__

#include "FireBall.h"

/**
 Represents the hero fire ball (laser) in the game. The class will render the fire ball (the super class will be responsible for the movement & state).
 */
class HeroFireBall : public FireBall
{
private:
    CCPoint _initialFirePosition;
    bool _startedFadeOutAnimation;
    float _fireBallElapsedCounter;
    CCPoint _lastScreenPosition;
    
public:
    HeroFireBall(CCPoint initialFirePosition, uint32_t fromPowerUpId, b2World* world);
    static HeroFireBall* create(CCPoint initialFirePosition, uint32_t fromPowerUpId, b2World* world);
    
	virtual void update (float dt);
    
};
#endif /* defined(__UFORun__HeroFireBall__) */
