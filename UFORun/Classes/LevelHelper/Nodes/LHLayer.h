//
//  LHLayer.h
//  ParallaxTimeBased
//
//  Created by Bogdan Vladu on 4/2/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//

#ifndef __LHLAYER_NODE__
#define __LHLAYER_NODE__


#include "cocos2d.h"
using namespace cocos2d;

class LevelHelperLoader;
class LHSprite;
class LHBatch;
class LHBezier;
class LHDictionary;
class LHAbstractClass;

class LHLayer : public CCLayer
{
public:
    virtual bool initWithDictionary(LHDictionary* dictionary);
    virtual ~LHLayer(void);
    LHLayer();
    
    static LHLayer* layerWithDictionary(LHDictionary* dict);
    void removeSelf(); //will also remove all children
    
    LevelHelperLoader* parentLoader();
    
    const std::string& getUniqueName(void){return uniqueName;}

    bool getIsMainLayer(void){return isMainLayer;}
    void setIsMainLayer(bool m){isMainLayer = m;}
    
    LHLayer*    layerWithUniqueName(const std::string& name); //does not return self
    LHBatch*    batchWithUniqueName(const std::string& name);
    LHSprite*   spriteWithUniqueName(const std::string& name);
    LHBezier*   bezierWithUniqueName(const std::string& name);
    
    CCArray*    allLayers(void); //does not return self
    CCArray*    allBatches(void);
    CCArray*    allSprites(void);
    CCArray*    allBeziers(void);
    
    CCArray*    layersWithTag(int tag); //does not return self
    CCArray*    batchesWithTag(int tag);
    CCArray*    spritesWithTag(int tag);
    CCArray*    beziersWithTag(int tag);

    static bool isLHLayer(CCNode* node);

    //USER DATA
    //--------------------------------------------------------------------------
    //will return "No Class" if no class is defined
    //will return the class name if a class is assigned to this sprite
    std::string userInfoClassName();
    
    //this will return an instance of the class defined in LH under Custom Class Properties
    //check for NULL to see if you have any info
    //use the class properties to read all your info
    //e.g MyClass* myInfo = (MyClass*)sprite->userInfo();  if(myInfo){ int life = myInfo.life); }
    
    //use the class properties to set new (other then the one set in LH) values
    //e.g MyClass* myInfo = (MyClass*)sprite->userInfo(); if(myInfo){ myInfo.life = 40; } )
    void* userInfo();
    
    
    virtual void draw(void);
private:    
    friend class LevelHelperLoader;
    
    LHAbstractClass* userCustomInfo;
    void loadUserCustomInfoFromDictionary(LHDictionary* dictionary);
    
    bool isMainLayer;
    std::string uniqueName;
    LevelHelperLoader* m_parentLoader;
    
    void setParentLoader(LevelHelperLoader* p){m_parentLoader = p;}

    void addChildFromDictionary(LHDictionary* childDict);
};

#endif