//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__PowerUp__
#define __UFORun__PowerUp__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

/**
 The supported power ups, extend this enum if you wish to add more power ups.
 */
typedef enum
{
    kPowerUpFireBall = 0,
    kPowerUpExtraSpeed,
    kPowerUpShield
} PowerUpType;

/**
 Define the supported power ups for the game.
 */
class PowerUp : public CCObject {
private:
    uint32_t _powerUpId; // The id is based on the appearance of the power up in the level (so the first power up's id is 1, the second is 2 and so on)
    CCPoint _powerUpScreenPosition;
    CCSize _powerUpContentSize;
    PowerUpType _powerUpType;
    CCNode *_powerUpNode; // weak ref
    
    PowerUp(uint32_t powerUpId, CCNode *powerUpNode, PowerUpType type);
public:
    static PowerUp* create(uint32_t powerUpId, CCNode *powerUpNode, PowerUpType type);
    
    static PowerUpType randomizePowerUpType();
    
    virtual uint32_t getPowerUpId(void) const{
        return _powerUpId;
    }
    
    virtual CCPoint getPowerUpPosition(void) const {
        return _powerUpScreenPosition;
    }
    
    virtual CCSize getPowerUpContentSize(void) const {
        return _powerUpContentSize;
    }
    
    virtual PowerUpType getPowerUpType(void) const {
        return _powerUpType;
    }
    
    virtual CCNode* getPowerUpNode(void) const {
        return _powerUpNode;
    }
    
    virtual bool isHit(CCNode *projectile);
};

#endif /* defined(__UFORun__PowerUp__) */
