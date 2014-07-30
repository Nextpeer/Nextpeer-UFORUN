//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "GameClock.h"

GameClock* GameClock::_instance = 0;

GameClock::GameClock() : _clock(0)
{
    
}

GameClock* GameClock::getInstance()
{
    if (!_instance) {
        _instance = new GameClock();
    }
    
    return _instance;
}

void GameClock::update(float dt)
{
    _clock += dt;
}

void GameClock::reset()
{
    _clock = 0;
}
