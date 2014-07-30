//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "GameLevel.h"
#include "Rand.h"

LevelHelperLoader* GameLevel::getRandomLevelLoader() {
    return getLevelLoaderByType(randomizeLevel());
}

GameLevelType GameLevel::randomizeLevel() {
    
    // Use Nextpeer random generator, this will keep the random value the same cross devices.
    // Nextpeer takes care of the seeding of that random generator, therefore all players will get the same powerup (at the same position).
    return static_cast<GameLevelType>(nextpeer::Rand::generate() % kGameLevelTypeCount);
}

LevelHelperLoader* GameLevel::getLevelLoaderByType(GameLevelType type) {
    
    const char* levelName = NULL;
        // TODO: RESTORE ME
    switch (type) {
        case kGameLevelTypeSnowWorld:
            levelName = "level01.plhs";
            break;
        default:
            levelName = "level00.plhs";
            break;
    }
    
    return new LevelHelperLoader(levelName);
}