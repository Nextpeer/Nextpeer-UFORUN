//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__GameContactListener__
#define __UFORun__GameContactListener__

#include "Box2D.h"
#include "cocos2d.h"
USING_NS_CC;

/**
 This contact listener will be used to verifiy if the hero is touching the ground or not.
 Read more here http://www.iforce2d.net/b2dtut/jumpability
 */
class GameContactListener : public b2ContactListener {
    public:
    GameContactListener();
    
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    CC_SYNTHESIZE(int, _numFootContacts, FootContacts);
};

#endif /* defined(__UFORun__GameContactListener__) */