//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__NextpeerDelegate__
#define __UFORun__NextpeerDelegate__

#include "cocos2d.h"
USING_NS_CC;

/**
 This class listen to the different events of the Nextpeer platform.
 */
class NextpeerDelegate : public CCObject
{
public:
    void registerForEvents();
    void unhookEvents();
    
    // Event calls
    void nextpeerDidStartTournament(CCObject* startData);
    void nextpeerDidEndTournament();
    
    NextpeerDelegate();
    virtual ~NextpeerDelegate();
};

#endif /* defined(__UFORun__NextpeerDelegate__) */