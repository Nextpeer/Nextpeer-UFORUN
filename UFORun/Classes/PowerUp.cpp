//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "PowerUp.h"

#define RANDOM_POWER_UP_FIRE_PERCENT 0.7
#define RANDOM_POWER_UP_SHIELD_PERCENT 0.2
#define RANDOM_FULL_PERCENT 1000

PowerUp* PowerUp::create(uint32_t powerUpId, CCNode *powerUpNode, PowerUpType type) {
    PowerUp *powerUp = new PowerUp(powerUpId, powerUpNode, type);
    powerUp->autorelease();
    return powerUp;
}

PowerUpType PowerUp::randomizePowerUpType() {
    // Use Nextpeer random generator, this will keep the random value the same cross devices.
    // Nextpeer takes care of the seeding of that random generator, therefore all players will get the same powerup (at the same position).
    // int powerUpPercent = nextpeer::Rand::generate() % RANDOM_FULL_PERCENT;
    
    // Use the Cocos2d-x random function to mis the powerups across devices.
    int powerUpPercent = CCRANDOM_0_1() * RANDOM_FULL_PERCENT;
    
    if (powerUpPercent <= RANDOM_FULL_PERCENT * RANDOM_POWER_UP_FIRE_PERCENT) {
        return kPowerUpFireBall;
    }
    else if (powerUpPercent <= RANDOM_FULL_PERCENT * (RANDOM_POWER_UP_FIRE_PERCENT + RANDOM_POWER_UP_SHIELD_PERCENT)) {
        return kPowerUpShield;
    }
    
    return kPowerUpExtraSpeed;
}

PowerUp::PowerUp(uint32_t powerUpId, CCNode *powerUpNode, PowerUpType type) {
    _powerUpId = powerUpId;
    _powerUpScreenPosition = powerUpNode->getPosition();
    _powerUpContentSize = powerUpNode->getContentSize();
    _powerUpType = type;
    _powerUpNode = powerUpNode;
}

bool PowerUp::isHit(CCNode *projectile) {
    
    // If the powerup node is not available or not visible there will be no target hit
    if (_powerUpNode == NULL || !_powerUpNode->isVisible()) return false;
    
    return projectile->boundingBox().intersectsRect(_powerUpNode->boundingBox());
}