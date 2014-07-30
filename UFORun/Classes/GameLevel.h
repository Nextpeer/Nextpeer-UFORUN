//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__GameLevel__
#define __UFORun__GameLevel__

#include "LevelHelperLoader.h"

// Extend this enum if you wish to add more levels to the game!
typedef enum
{
    kGameLevelTypeSnowWorld = 0,
    kGameLevelTypeCount
    
} GameLevelType;

/**
 The class will randomize the same level for all devices based on Nextpeer random function (same seed to all connected devices).
 */
class GameLevel {
public:
    static LevelHelperLoader* getRandomLevelLoader();
    static GameLevelType randomizeLevel();
    static LevelHelperLoader* getLevelLoaderByType(GameLevelType type);
};


#endif /* defined(__UFORun__GameLevel__) */
