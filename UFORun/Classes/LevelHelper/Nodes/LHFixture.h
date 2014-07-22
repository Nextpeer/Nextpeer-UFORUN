//
//  LHFixture.h
//
//  Created by Bogdan Vladu on 4/3/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//
#ifndef __LH_FIXTURE_OBJ__
#define __LH_FIXTURE_OBJ__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
using namespace cocos2d;

class LHSprite;
class LHDictionary;

class LHFixture : public CCObject
{

public:
    
    virtual bool initWithDictionary(LHDictionary* dictionary,b2Body* body, LHSprite* sprite);
    virtual ~LHFixture();
    LHFixture();
    
    static LHFixture* fixtureWithDictionary(LHDictionary* dictionary, b2Body* body, LHSprite* sprite);
    
    //this class is added as userData to b2Fixture object - it is removed when the body is removed from the sprite (on sprite dealloc or or specific body destroy)
    //it should not be removed in any other way

    
    const std::string& getFixtureName(){return fixtureName;}
    const int getFixtureID(){return fixtureID;}

    b2Fixture* box2dFixture(){return m_box2dFixture;}
    
    static bool isLHFixture(CCObject* obj);
    static LHFixture* fixtureForb2Fixture(b2Fixture* fix);
    
private:
    std::string fixtureName;
    int fixtureID;
    b2Fixture* m_box2dFixture; //week ptr
    
    b2Vec2 transformPoint(CCPoint point, LHSprite* sprite, CCPoint offset, CCPoint scale);
};

#endif