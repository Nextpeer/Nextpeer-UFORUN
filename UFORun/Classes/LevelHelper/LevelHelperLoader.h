//  This file is part of LevelHelper
//  http://www.levelhelper.org
//
//  Created by Bogdan Vladu
//  Copyright 2011 Bogdan Vladu. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//  The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software. If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//  Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//  This notice may not be removed or altered from any source distribution.
//  By "software" the author refers to this code file and not the application 
//  that was used to generate this file.
//  You do not have permission to use this code or any part of it if you don't
//  own a license to LevelHelper application.
////////////////////////////////////////////////////////////////////////////////
//
//  Version history
//  ...............
//  v0.1 First version for LevelHelper 1.4.9x
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEVEL_HELPER_LOADER__
#define __LEVEL_HELPER_LOADER__


#include "lhConfig.h"
#include "cocos2d.h"
#include "cocoa/CCNS.h"
#include "Box2D/Box2D.h"

#include "Utilities/LHDictionary.h"
#include "Utilities/LHArray.h"
#include "Utilities/LHObject.h"

#include "Nodes/LHCustomSpriteMgr.h"
#include "Nodes/LHContactNode.h"
#include "Nodes/LHAnimationNode.h"
#include "Nodes/LHParallaxNode.h"
#include "Nodes/LHContactInfo.h"

#include "Nodes/LHNode.h"
#include "Nodes/LHLayer.h"
#include "Nodes/LHBatch.h"
#include "Nodes/LHSprite.h"
#include "Nodes/LHBezier.h"
#include "Nodes/LHJoint.h"
#include "Nodes/LHPathNode.h"

#include "CustomClasses/LHCustomClasses.h"

#include "Nodes/LHCuttingEngineMgr.h"

using namespace cocos2d;

enum LevelHelper_TAG 
{ 
	DEFAULT_TAG 	= 0,
	GAME_LEVEL_START_BLOCK 			= 1,
	GAME_LEVEL_START_FLAG 			= 2,
	GAME_LEVEL_END_FLAG 			= 3,
	GAME_LEVEL_POWERUP_BOX 			= 4,
	NUMBER_OF_TAGS 	= 5
};

std::string stringFromInt(const int& i);

typedef void (CCObject::*SEL_CallFuncFloat)(float);
#define callfuncFloat_selector(_SELECTOR) (SEL_CallFuncFloat)(&_SELECTOR)

class LH_b2DestructionListener;

class LevelHelperLoader : public CCObject {

private:
	LHArray* lhNodes;	//array of NSDictionary //includes LHSprite, LHBezier, LHBatch, LHLayer
    LHArray* lhJoints;	//array of NSDictionary
    LHArray* lhParallax;//array of NSDictionary 
    LHDictionary*  wb;//world boundaries info;
    
    LHLayer* mainLHLayer;
    
#if COCOS2D_VERSION >= 0x00020000
    
    CCDictionary jointsInLevel;
    CCDictionary parallaxesInLevel;    
    CCDictionary physicBoundariesInLevel;
#else
    CCMutableDictionary<std::string>  jointsInLevel;        //key name - value LHJoint*
    CCMutableDictionary<std::string>  parallaxesInLevel;    //key name - value LHParallaxNode*
    CCMutableDictionary<std::string>  physicBoundariesInLevel;
#endif
    
    CCPoint safeFrame;
    CCRect  gameWorldRect;
    CCPoint gravity;
	    
	CCLayer* cocosLayer; //weak ptr
    b2World* box2dWorld; //weak ptr
    
    LHContactNode* contactNode;
    
    LH_b2DestructionListener* destructionListener;
        
    CCObject* loadingProgressId;
    SEL_CallFuncFloat loadingProgressSel;
    
    bool m_isPaused;
public:
    
    //------------------------------------------------------------------------------
    LevelHelperLoader(const char* levelFile);
    virtual ~LevelHelperLoader();
    //------------------------------------------------------------------------------
    
    //will call this selector during loading the level (addObjectsToWorld or addSpritesToLayer)
    //the registered method needs to have this signature " void HelloWorld::loadingProgress(float percentage) "
    //registration should be done like this loader->registerLoadingProgressObserver(this, callfuncFloat_selector(HelloWorld::loadingProgress));
    //percentage will return a value from 0.0f to 1.0f
    void registerLoadingProgressObserver(CCObject* loadingProgressObj, SEL_CallFuncFloat sel);
    
    //LOADING
    void addObjectsToWorld(b2World* world, CCLayer* cocosLayer);
    
    
    LHLayer*  layerWithUniqueName(const std::string& name);
    LHBatch*  batchWithUniqueName(const std::string& name);
    LHSprite* spriteWithUniqueName(const std::string& name);
    LHBezier* bezierWithUniqueName(const std::string& name);
    LHJoint*  jointWithUniqueName(const std::string& name);
    LHParallaxNode* parallaxNodeWithUniqueName(const std::string& uniqueName);
    
    CCArray* allLayers();
    CCArray* allBatches();
    CCArray* allSprites();
    CCArray* allBeziers();
    CCArray* allJoints();
    CCArray* allParallaxes();
    
    CCArray* layersWithTag(enum LevelHelper_TAG tag);
    CCArray* batchesWithTag(enum LevelHelper_TAG tag);
    CCArray* spritesWithTag(enum LevelHelper_TAG tag);
    CCArray* beziersWithTag(enum LevelHelper_TAG tag);
    CCArray* jointsWithTag(enum LevelHelper_TAG tag);
    
    
    /*
     to remove any of the LHLayer, LHBatch, LHSprite, LHBezier, LHJoint objects call
     object->removeSelf();
     
     if you retain it somewhere the object will not be release - so make sure you dont retain
     any of the objects
     */

    
    //in cases where you cannot control when the box2d world is deleted
    //you may want to call this method prior releasing the LevelHelperLoader instance so that you
    //dont get an crash when cocos2d removes the sprites and box2d world object is no longer alive
    void removeAllPhysics();

    
    
    
    
    
    
//    void addSpritesToLayer(CCLayer* cocosLayer); //NO PHYSICS
    //------------------------------------------------------------------------------
    LH_DEPRECATED_ATTRIBUTE static void dontStretchArtOnIpad(void);
    static void dontStretchArt(void);

    //UTILITIES
    //The offset value is not transformed and is device dependend
    //You must apply this offset based on device. To test for the device you eighter use one of the
    //ios specific method to get the device or test for the winSize using CCDirector
    //for iPhone5 if you want to make the level to be positioned starting from the left-centered corner rather then in the center
    //the offset should be ccp(-88, 0)
    //for iPad if you want to make the level to be positioned starting from the left-centered corner rather then in the center
    //the offset should be ccp(-32,0);
    //left-top corner should be ccp(-32,-64);
    //THIS METHOD SHOULD BE USED TOGETHER WITH dontStretchArt
    static void loadLevelsWithOffset(CCPoint offset);
    
    
    //------------------------------------------------------------------------------
    //PAUSING THE GAME
    //this will pause all path movement and all parallaxes
    //use  [[CCDirector sharedDirector] pause]; for everything else
    static bool isPaused(void);
    static void setPaused(bool value); //pass true to pause, false to unpause

    //this will pause only the current loader instance and not all the instances like the static methods above
    bool getIsPaused(void);
    void setIsPaused(bool value);
    
    //------------------------------------------------------------------------------
    //COLLISION HANDLING
    //see API Documentation on the website to see how to use this
    void useLevelHelperCollisionHandling(void);
    
    void registerBeginOrEndCollisionCallbackBetweenTagA(enum LevelHelper_TAG tagA,
                                                        enum LevelHelper_TAG tagB,
                                                        CCObject* obj,
                                                        SEL_CallFuncO selector);
    
    void cancelBeginOrEndCollisionCallbackBetweenTagA(enum LevelHelper_TAG tagA,
                                                      enum LevelHelper_TAG tagB);

    
    void registerPreCollisionCallbackBetweenTagA(enum LevelHelper_TAG tagA,
                                                enum LevelHelper_TAG tagB,
                                                CCObject* obj,
                                                SEL_CallFuncO selector);
    
    void cancelPreCollisionCallbackBetweenTagA(enum LevelHelper_TAG tagA,
                                               enum LevelHelper_TAG tagB);
    
    void registerPostCollisionCallbackBetweenTagA(enum LevelHelper_TAG tagA,
                                                 enum LevelHelper_TAG tagB,
                                                 CCObject* obj,
                                                 SEL_CallFuncO selector);
    
    void cancelPostCollisionCallbackBetweenTagA(enum LevelHelper_TAG tagA,
                                                enum LevelHelper_TAG tagB);

    
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    //CREATION
    
    
    
    //SPRITE CREATION
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    //name is from one of a sprite already in the level
    //parent will be Main Layer
    //if you use custom sprite classes - this will create a sprite of that custom registered class
    //method will create custom sprite if one is register for the tag of this sprite
    LHSprite* createSpriteWithUniqueName(const std::string& name);
    
    //use this method if you want the sprite to be child of a specific node and not the main LH node
    //pass nil if you dont want a parent
    //method will create custom sprite if one is register for the tag of this sprite
    LHSprite* createSpriteWithUniqueName(const std::string& name, CCNode* parent);
    
    //name is from one of a sprite already in the level
    //parent will be the batch node that is handling the image file of this sprite
    //method will create custom sprite if one is register for the tag of this sprite
    LHSprite* createBatchSpriteWithUniqueName(const std::string& name);
    
    
    LHSprite* createSpriteWithName(const std::string& name,
                                   const std::string& shSheetName,
                                   const std::string& shFile);
    
    //use this method if you want the sprite to be child of a specific node and not the main LH node
    //pass nil if you dont want a parent
    LHSprite* createSpriteWithName(const std::string& name,
                                   const std::string& shSheetName,
                                   const std::string& shFile,
                                   CCNode* parent);
    
    
    //use this in order to create sprites of custom types
    LHSprite* createSpriteWithName(const std::string& name,
                                   const std::string& shSheetName,
                                   const std::string& shFile,
                                   LevelHelper_TAG tag);
    
    //use this method if you want the sprite to be child of a specific node and not the main LH node
    //pass nil if you dont want a parent
    LHSprite* createSpriteWithName(const std::string& name,
                                   const std::string& shSheetName,
                                   const std::string& shFile,
                                   LevelHelper_TAG tag,
                                   CCNode* parent);
    
    
    LHSprite* createBatchSpriteWithName(const std::string& name,
                                        const std::string& shSheetName,
                                        const std::string& shFile);
    
    //use this in order to create sprites of custom types
    LHSprite* createBatchSpriteWithName(const std::string& name,
                                        const std::string& shSheetName,
                                        const std::string& shFile,
                                        LevelHelper_TAG tag);
    
    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    
//    //------------------------------------------------------------------------------
//    //PARALLAX
    void removeParallaxNode(LHParallaxNode* node, bool removeSprites = false);
    void removeAllParallaxes(bool removeSprites = false); //does not remove the sprites
//    //------------------------------------------------------------------------------
//    //------------------------------------------------------------------------------

    //GRAVITY
    bool isGravityZero(void);
    void createGravity(b2World* world);
    //------------------------------------------------------------------------------
//    //PHYSIC BOUNDARIES
    void createPhysicBoundaries(b2World* _world);

    //this method should be used when using dontStretchArtOnIpad
    //see api documentatin for more info
    void createPhysicBoundariesNoStretching(b2World * _world);

    CCRect physicBoundariesRect(void);
    bool hasPhysicBoundaries(void);

    b2Body* leftPhysicBoundary(void);
    LHNode* leftPhysicBoundaryNode(void);
    b2Body* rightPhysicBoundary(void);
    LHNode* rightPhysicBoundaryNode(void);
    b2Body* topPhysicBoundary(void);
    LHNode* topPhysicBoundaryNode(void);
    b2Body* bottomPhysicBoundary(void);
    LHNode* bottomPhysicBoundaryNode(void);
    void removePhysicBoundaries(void);
    
//    //------------------------------------------------------------------------------
//    //LEVEL INFO
    CCSize gameScreenSize(void); //the device size set in loaded level
    CCRect gameWorldSize(void); //the size of the game world
//    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    //PHYSICS
    static void setMeterRatio(float ratio); //default is 32.0f
    static float meterRatio(void); //same as pointsToMeterRatio - provided for simplicity as static method
    
    static float pixelsToMeterRatio(void);
    static float pointsToMeterRatio(void);
    
    static b2Vec2 pixelToMeters(CCPoint point); //Cocos2d point to Box2d point
    static b2Vec2 pointsToMeters(CCPoint point); //Cocos2d point to Box2d point
    
    static CCPoint metersToPoints(b2Vec2 vec); //Box2d point to Cocos2d point
    static CCPoint metersToPixels(b2Vec2 vec); //Box2d point to Cocos2d pixels
    
private:
    
    void initObjects(void);
    
    friend class LHJoint;
    friend class LHSprite;
    friend class LHBezier;
    friend class LHLayer;
    friend class LHCuttingEngineMgr;

    
    void createAllNodes();
    void createAllJoints();
    
    LHParallaxNode*  parallaxNodeFromDictionary(LHDictionary* parallaxDict, CCLayer*layer);
    void createParallaxes();
    void startAllPaths();

    b2World* getPhysicsWorld();
    void removeMainLayer();
    
    void removeJoint(LHJoint* jt);
    void callLoadingProgressObserverWithValue(float val);
    
    void loadLevelHelperSceneFile(const char* levelFile,
                                  const char* subfolder, 
                                  const char* imgFolder);

    void loadLevelHelperSceneFromDictionary(const LHDictionary& levelDictionary,
                                            const std::string& imgFolder);


    void processLevelFileFromDictionary(LHDictionary* dictionary);

    LHDictionary* dictionaryInfoForSpriteNodeNamed(const std::string& name, LHDictionary* dict);
    
    static void setTouchDispatcherForBezierWithTag(LHBezier* object, int tag);
    static void setTouchDispatcherForSpriteWithTag(LHSprite* object, int tag);
    static void removeTouchDispatcherFromSprite(LHSprite* object);
    static void removeTouchDispatcherFromBezier(LHBezier* object);
    
    b2Body* physicBoundarieForKey(const std::string& key);
    void setFixtureDefPropertiesFromDictionary(LHDictionary* spritePhysic, b2FixtureDef* shapeDef);
    void createPhysicBoundariesHelper(b2World* _world,
                                      const CCPoint& wbConv,
                                      const CCPoint& pos_offset);

};

#endif















