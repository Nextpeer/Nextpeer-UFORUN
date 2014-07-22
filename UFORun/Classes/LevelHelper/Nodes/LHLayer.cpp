//
//  LHLayer.m
//  ParallaxTimeBased
//
//  Created by Bogdan Vladu on 4/2/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//

#include "LHLayer.h"
#include "LHBatch.h"
#include "LHSprite.h"
#include "LHBezier.h"
#include "LHSettings.h"
#include "../LevelHelperLoader.h"
#include "../Utilities/LHDictionary.h"

#include "../CustomClasses/LHAbstractClass.h"
#include "../CustomClasses/LHCustomClasses.h"
static int untitledLayersCount = 0;

void LHLayer::loadUserCustomInfoFromDictionary(LHDictionary* dictionary){
    userCustomInfo = NULL;
    
    if(!dictionary)return;
    
    std::string className = dictionary->stringForKey("ClassName");
    
    userCustomInfo = LHCustomClassesMgr::customClassInstanceWithName(className);
    
    if(!userCustomInfo) return;
    
    LHDictionary* dict = dictionary->dictForKey("ClassRepresentation");
    
    if(dict){
        //        CCLog("SETTING PROPERTIES FROM DICT");
        ((LHAbstractClass*)userCustomInfo)->setPropertiesFromDictionary(dict);
    }
}
bool LHLayer::initWithDictionary(LHDictionary* dictionary){
    
    isMainLayer = false;
    
    uniqueName = dictionary->stringForKey("UniqueName");
    
    if(uniqueName == ""){
        uniqueName = "UntitledLayer_" + stringFromInt(untitledLayersCount);
        ++untitledLayersCount;
    }
    
    setTag(dictionary->intForKey("Tag"));
    m_nZOrder = dictionary->intForKey("ZOrder");
    
    loadUserCustomInfoFromDictionary(dictionary->dictForKey("CustomClassInfo"));
    
    LHArray* childrenInfo = dictionary->arrayForKey("Children");
    for(int i = 0; i< childrenInfo->count(); ++i){
        LHDictionary* childDict = childrenInfo->dictAtIndex(i);
        addChildFromDictionary(childDict);
    }
    
    return true;
}
//------------------------------------------------------------------------------
LHLayer::~LHLayer(void){
//   printf("LH Layer Dealloc %s\n", uniqueName.c_str());
    if(userCustomInfo){
        delete userCustomInfo;
        userCustomInfo = NULL;
    }
}
//------------------------------------------------------------------------------
LHLayer::LHLayer(){
    
}
//------------------------------------------------------------------------------
LHLayer* LHLayer::layerWithDictionary(LHDictionary* dict){
    LHLayer *pobNode = new LHLayer();
	if (pobNode && pobNode->initWithDictionary(dict))
    {
	    pobNode->autorelease();
        return pobNode;
    }
    CC_SAFE_DELETE(pobNode);
	return NULL;
}
//------------------------------------------------------------------------------
void LHLayer::removeSelf(){
    
    if(isMainLayer)
    {
        CCLog("LevelHelper ERROR: MAIN_LAYER cannot be removed with removeSelf(). You will need to delete the entire LevelHelperLoader object.");
        return;
    }

    if(m_parentLoader->getPhysicsWorld()){
        if(m_parentLoader->getPhysicsWorld()->IsLocked()){
            LHSettings::sharedInstance()->markNodeForRemoval(this);
            return;
        }
    }

    m_parentLoader->removeMainLayer();
    removeFromParentAndCleanup(true);
}
LevelHelperLoader* LHLayer::parentLoader(){
    return m_parentLoader;
}
//------------------------------------------------------------------------------
bool LHLayer::isLHLayer(CCNode* node){
    if( 0 != dynamic_cast<LHLayer*>(node))
        return true;
    return false;
}
std::string LHLayer::userInfoClassName(){
    if(userCustomInfo)
        return ((LHAbstractClass*)userCustomInfo)->className();
    return "No Class";
}
//------------------------------------------------------------------------------
void* LHLayer::userInfo(){
    return userCustomInfo;
}

//------------------------------------------------------------------------------
LHLayer*    LHLayer::layerWithUniqueName(const std::string& name){
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        if(LHLayer::isLHLayer(node)){
            if(((LHLayer*)node)->getUniqueName() == name){
            return (LHLayer*)node;
            }
        }
    }
    return NULL;
}
//------------------------------------------------------------------------------
LHBatch*    LHLayer::batchWithUniqueName(const std::string& name){
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        if(LHBatch::isLHBatch(node)){
            if(((LHBatch*)node)->getUniqueName() == name){
                return (LHBatch*)node;
            }
        }
        else if(LHLayer::isLHLayer(node)){
            LHBatch* child = ((LHLayer*)node)->batchWithUniqueName(name);
            if(child)
                return child;
        }
    }
    return NULL; 
}
//------------------------------------------------------------------------------
LHSprite*   LHLayer::spriteWithUniqueName(const std::string& name){
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        if(LHSprite::isLHSprite(node)){
            if(((LHSprite*)node)->getUniqueName() == name){
                return (LHSprite*)node;
            }
        }
        else if(LHBatch::isLHBatch(node)){
            LHSprite* child = ((LHBatch*)node)->spriteWithUniqueName(name);
            if(child)
                return child;
        }
        else if(LHLayer::isLHLayer(node)){
            LHSprite* child = ((LHLayer*)node)->spriteWithUniqueName(name);
            if(child)
                return child;
        }
    }
    return NULL;    
}
//------------------------------------------------------------------------------
LHBezier*   LHLayer::bezierWithUniqueName(const std::string& name){
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        if(LHBezier::isLHBezier(node)){
            if(((LHBezier*)node)->getUniqueName() == name)
                return (LHBezier*)node;
        }
        else if(LHLayer::isLHLayer(node)){
            LHBezier* child = ((LHLayer*)node)->bezierWithUniqueName(name);
            if(child)
                return child;
        }
    }
    return NULL;    
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::allLayers(void){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        if(LHLayer::isLHLayer(node)){
            array->addObject(node);
        }
    }
    return array;
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::allBatches(void){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        
        if(LHBatch::isLHBatch(node)){
            array->addObject(node);
        }
        else if(LHLayer::isLHLayer(node)){
            array->addObjectsFromArray(((LHLayer*)node)->allBatches());
        }
    }
    return array; 
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::allSprites(void){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        
        if(LHSprite::isLHSprite(node)){
            array->addObject(node);
        }
        else if(LHBatch::isLHBatch(node)){
            array->addObjectsFromArray(((LHBatch*)node)->allSprites());
        }
        else if(LHLayer::isLHLayer(node)){
            array->addObjectsFromArray(((LHLayer*)node)->allSprites());
        }
    }
    return array;   
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::allBeziers(void){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);

        if(LHBezier::isLHBezier(node)){
            array->addObject(node);
        }
        else if(LHLayer::isLHLayer(node)){
            array->addObjectsFromArray(((LHLayer*)node)->allBeziers());
        }
    }
    return array;    
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::layersWithTag(int tag){
    
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);

        if(LHLayer::isLHLayer(node)){
            if(node->getTag() == tag)
                array->addObject(node);
        }
    }
    return array;   
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::batchesWithTag(int tag){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);

        if(LHBatch::isLHBatch(node)){
            if(node->getTag() == tag)
                array->addObject(node);
        }
        else if(LHLayer::isLHLayer(node)){
            array->addObjectsFromArray(((LHLayer*)node)->batchesWithTag(tag));
        }
    }
    return array;       
}
//------------------------------------------------------------------------------
CCArray*    LHLayer::spritesWithTag(int tag){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        
        if(LHSprite::isLHSprite(node)){
            if(node->getTag() == tag)
                array->addObject(node);
        }
        else if(LHBatch::isLHBatch(node)){
            array->addObjectsFromArray(((LHBatch*)node)->spritesWithTag(tag));
        }
        else if(LHLayer::isLHLayer(node)){
            array->addObjectsFromArray(((LHLayer*)node)->spritesWithTag(tag));
        }
    }
    return array;       
}
//------------------------------------------------------------------------------
CCArray* LHLayer::beziersWithTag(int tag){
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    CCArray* children = getChildren();
    for(int i = 0; i < children->count(); ++i){
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        
        if(LHBezier::isLHBezier(node)){
            if(node->getTag() == tag)
                array->addObject(node);
        }
        else if(LHLayer::isLHLayer(node)){
            array->addObjectsFromArray(((LHLayer*)node)->beziersWithTag(tag));
        }
    }
    return array;   
}
//------------------------------------------------------------------------------
void LHLayer::draw(void){
    CCLayer::draw(); // calls base class' function
    if(isMainLayer){
        LHSettings::sharedInstance()->removeMarkedNodes();
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LHLayer::addChildFromDictionary(LHDictionary* childDict)
{
    if(childDict->stringForKey("NodeType") == "LHSprite")
    {
        LHDictionary* texDict = childDict->dictForKey("TextureProperties");
        int sprTag = texDict->intForKey("Tag");
        
        lh_spriteCreationMethods methods = LHCustomSpriteMgr::sharedInstance()->customSpriteClassForTag(sprTag);
        
        LHSprite* sprite =  (*methods.first)(childDict); //spriteWithDictionary
        addChild(sprite);
        sprite->postInit();
    }
    else if(childDict->stringForKey("NodeType") == "LHBezier")
    {
        LHBezier* bezier = LHBezier::bezierWithDictionary(childDict);
        addChild(bezier);
        //we use the selector protocol so that we dont get warnings since this method is 
        //hidden from the user
    }
    else if(childDict->stringForKey("NodeType") == "LHBatch"){
       /* LHBatch* batch =*/ LHBatch::batchWithDictionary(childDict, this);
        //it adds self in the layer //this is needed for animations
        //we need to have the layer parent before creating the sprites
    }
    else if(childDict->stringForKey("NodeType") == "LHLayer"){
        LHLayer* layer = LHLayer::layerWithDictionary(childDict);
        addChild(layer, layer->getZOrder());
    }
}