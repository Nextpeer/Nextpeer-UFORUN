//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "GameContactListener.h"
#include "Hero.h"

GameContactListener::GameContactListener() {
    _numFootContacts = 0;
}

void GameContactListener::BeginContact(b2Contact* contact) {
    
    //check if fixture A was the foot sensor
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    if (*(int *)fixtureUserData == HERO_FOOT) {
        _numFootContacts++;
    }
    
    //check if fixture B was the foot sensor
    fixtureUserData = contact->GetFixtureB()->GetUserData();
    if (*(int *)fixtureUserData == HERO_FOOT) {
        _numFootContacts++;
    }
}

void GameContactListener::EndContact(b2Contact* contact) {
    
    //check if fixture A was the foot sensor
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    if (*(int *)fixtureUserData == HERO_FOOT) {
        _numFootContacts--;
    }
    //check if fixture B was the foot sensor
    fixtureUserData = contact->GetFixtureB()->GetUserData();
    if (*(int *)fixtureUserData == HERO_FOOT) {
        _numFootContacts--;
    }
}