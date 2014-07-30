//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__GameClock__
#define __UFORun__GameClock__

#include "cocos2d.h"
USING_NS_CC;

/**
 The game clock assists the opponent interpolation calculations (between updates).
 */
class GameClock
{
private:
    static GameClock* _instance;
    
    GameClock();
    
public:
    static GameClock* getInstance();
    void update(float dt);
    void reset();
    
    CC_SYNTHESIZE_READONLY(float, _clock, Clock);
};

#endif /* defined(__UFORun__GameClock__) */
