//
//  LHFixture.mm
//
//  Created by Bogdan Vladu on 4/3/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//

#include "LHFixture.h"
#include "../Utilities/LHDictionary.h"
#include "LHSettings.h"
#include "../LevelHelperLoader.h"
#include "LHSprite.h"

//------------------------------------------------------------------------------
b2Vec2 LHFixture::transformPoint(CCPoint point, LHSprite* sprite, CCPoint offset, CCPoint scale)
{
    float ptm = LHSettings::sharedInstance()->lhPtmRatio();
    
    int flipx = sprite->isFlipX() ? -1.f : 1.f;
    int flipy = sprite->isFlipY() ? -1.f : 1.f;    
    
    point.x *= scale.x*flipx;
    point.y *= scale.y*flipy;
    
    if(LHSettings::sharedInstance()->isHDImage(sprite->getImageFile())){
        point.x *=2.0f;
        point.y *=2.0f;
    }
    return b2Vec2(point.x/ptm, point.y/ptm);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool LHFixture::initWithDictionary(LHDictionary* dictionary,b2Body* body, LHSprite* sprite)
{
    fixtureName = std::string(dictionary->stringForKey("Name"));
    fixtureID   = dictionary->intForKey("Tag");
 
    int category= dictionary->intForKey("Category");
    int group   = dictionary->intForKey("Group");
    int mask    = dictionary->intForKey("Mask");
        
    float density       = dictionary->floatForKey("Density");
    float friction      = dictionary->floatForKey("Friction");
    float restitution   = dictionary->floatForKey("Restitution");
        
    bool isCircle = dictionary->boolForKey("IsCircle");
    bool isSensor = dictionary->boolForKey("IsSensor");
        
    CCPoint offset  = dictionary->pointForKey("LHShapePositionOffset");
    float width     = dictionary->floatForKey("LHWidth");
    float height    = dictionary->floatForKey("LHHeight");
        
    LHArray* fixturePoints = dictionary->arrayForKey("Fixture");
        
    float ptm = LHSettings::sharedInstance()->lhPtmRatio();
        
    CCPoint scale = ccp(sprite->getScaleX(), sprite->getScaleY());
    int flipx = sprite->isFlipX() ? -1 : 1;
    int flipy = sprite->isFlipY() ? -1 : 1;

//    if(fixturePoints && fixturePoints->count() > 0)
//    {
//        if(fixturePoints->objectAtIndex(0)->type() != LHObject::LH_ARRAY_TYPE)
//        {
//            CCLog("ERROR: Please update to SpriteHelper 1.8.x and resave all your scenes. Body will be created without a shape.");
//            return false;
//        }
//    }
    
    if(fixturePoints && fixturePoints->count() > 0 && fixturePoints->arrayAtIndex(0)->count() > 0)
    {
        for(int f = 0; f < fixturePoints->count(); ++f)
        {
            LHArray* fixInfo = fixturePoints->arrayAtIndex(f);
//
//            if(fixInf->type() != LHObject::LH_ARRAY_TYPE)
//            {
//                CCLog("ERROR: Please update to SpriteHelper 1.8.1 and resave all your scenes. Body will be created without a shape.");
//                break;
//            }
            
//            LHArray* fixInfo = fixInf->arrayValue();
            
            int count = fixInfo->count();
            b2Vec2 *verts = new b2Vec2[count];
            b2PolygonShape shapeDef;
            int i = 0;            

            for(int j = 0; j< count; ++j)
            {
                const int idx = (flipx < 0 && flipy >= 0) || (flipx >= 0 && flipy < 0) ? count - i - 1 : i;
                    
                CCPoint point = fixInfo->pointAtIndex(j);
                    
                verts[idx] = transformPoint(point, sprite, offset, scale);
                    
                ++i;
            }
            
            shapeDef.Set(verts, count);		
                
            b2FixtureDef fixture;
            //------------------------------------------------------------------
            fixture.density = density;
            fixture.friction = friction;
            fixture.restitution = restitution;
                
            fixture.filter.categoryBits = category;
            fixture.filter.maskBits = mask;
            fixture.filter.groupIndex = group;
                
            fixture.isSensor = isSensor;
                
            fixture.userData = this;

            //------------------------------------------------------------------            
            fixture.shape = &shapeDef;
            b2Fixture* finalFixture = body->CreateFixture(&fixture);
            finalFixture->SetUserData(this);
            delete[] verts;
        }
    }
    else    
    {    //handle isCircle and quads

        b2PolygonShape shape;
        b2FixtureDef fixture;
        b2CircleShape circle;

        //------------------------------------------------------------------
        fixture.density = density;
        fixture.friction = friction;
        fixture.restitution = restitution;
            
        fixture.filter.categoryBits = category;
        fixture.filter.maskBits = mask;
        fixture.filter.groupIndex = group;
        
        fixture.isSensor = isSensor;

        fixture.userData = this;
        //------------------------------------------------------------------            
            
        CCPoint origin = ccp(offset.x*scale.x*flipx,  -offset.y*scale.y*flipy);
//        if(LHSettings::sharedInstance()->isHDImage(sprite->getImageFile()))
//        {
//            origin.x *=2.0f;
//            origin.y *=2.0f;
//        }
        
        if(isCircle)
        {
            if(LHSettings::sharedInstance()->convertLevel()){
                //circle look weird if we dont do this
                float scaleSpr = sprite->getScaleX();
                sprite->setScaleY(scaleSpr);
            }
                
            float circleScale = scale.x; //if we dont do this we dont have collision
            if(circleScale < 0)
                circleScale = -circleScale;
                
            float radius = (width*circleScale)/ptm;
                
            if(!LHSettings::sharedInstance()->isHDImage(sprite->getImageFile())){
                radius /=2.0f;
            }

            if(radius < 0)
                radius *= -1;
            circle.m_radius = radius; 
                
            circle.m_p.Set( origin.x/ptm,
                            origin.y/ptm);
                
            fixture.shape = &circle;
            b2Fixture* finalFixture = body->CreateFixture(&fixture);
            finalFixture->SetUserData(this);
        }
        else
        {
            //this is for the case where no shape is defined and user selects the sprite to have physics inside LH
            b2PolygonShape shape;
                
            float boxWidth = width*scale.x/2.f;
            float boxHeight= height*scale.y/2.f;
                
//            if(LHSettings::sharedInstance()->isHDImage(sprite->getImageFile()))
//            {
//                boxWidth *=2.0f;
//                boxHeight *=2.0f;
//            }
            
            shape.SetAsBox( boxWidth/ptm, 
                            boxHeight/ptm);
                                
            shape.SetAsBox(boxWidth/ptm, boxHeight/ptm, b2Vec2(origin.x/ptm, origin.y/ptm), 0);

            fixture.shape = &shape;
            b2Fixture* finalFixture = body->CreateFixture(&fixture);
            finalFixture->SetUserData(this);
        }
    }                
    return true;
}
//------------------------------------------------------------------------------
LHFixture::LHFixture()
{
    
}
//------------------------------------------------------------------------------
LHFixture* LHFixture::fixtureWithDictionary(LHDictionary* dictionary, b2Body* body, LHSprite* sprite)
{
    LHFixture *pobNode = new LHFixture();
	if (pobNode && pobNode->initWithDictionary(dictionary,body,sprite))
    {
	    pobNode->autorelease();
        return pobNode;
    }
    CC_SAFE_DELETE(pobNode);
	return NULL;
}

bool LHFixture::isLHFixture(CCObject* obj){
    
    if( 0 != dynamic_cast<LHFixture*>(obj))
        return true;
    
    return false;
}

LHFixture* LHFixture::fixtureForb2Fixture(b2Fixture* fix){
    
    if(0 == fix)
        return 0;
    
    LHFixture* lhFix = (LHFixture*)fix->GetUserData();
    
    if(LHFixture::isLHFixture(lhFix))
    {
        return (LHFixture*)lhFix;
    }
    
    return 0;
}

//------------------------------------------------------------------------------
LHFixture::~LHFixture()
{
    //NSLog(@"LH FIXTURE DEALLOC %@", fixtureName);
}
//------------------------------------------------------------------------------
