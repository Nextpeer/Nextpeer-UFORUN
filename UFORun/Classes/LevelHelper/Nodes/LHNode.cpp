//
//  LHNode.m
//  
//
//  Created by Bogdan Vladu on 4/2/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//

#include "LHNode.h"
#include "../Utilities/LHDictionary.h"
#include "../LevelHelperLoader.h"

//------------------------------------------------------------------------------
static int untitledNodesCount = 0;
//------------------------------------------------------------------------------
LHNode::~LHNode(){
//    printf("LHNode dealloc %s\n", uniqueName.c_str());
    removeBodyFromWorld();
}
//------------------------------------------------------------------------------
bool LHNode::initWithDictionary(LHDictionary* dictionary){
    
    uniqueName = dictionary->stringForKey("UniqueName");
    
    if(uniqueName == "")
    { 
        uniqueName = "UntitledNode_" + stringFromInt(untitledNodesCount);
        ++untitledNodesCount;
    }
    
    LHArray* childrenInfo = dictionary->arrayForKey("Children");
    
    if(childrenInfo)
    for(int i = 0; i < childrenInfo->count(); ++i)
    {
        LHDictionary* childDict = childrenInfo->dictAtIndex(i);

        if(childDict->stringForKey("NodeType") == "LHLayer")
        {
            //nothing to do yet.
        }
        else if(childDict->stringForKey("NodeType") == "LHBatch")
        {
            //nothing to do yet.
        }
        else if(childDict->stringForKey("NodeType") == "LHBezier")
        {
            //nothing to do yet.
        }
        else if(childDict->stringForKey("NodeType") == "LHSprite")
        {
            //nothing to do yet.
        }
    }
    
    return true;
}
//------------------------------------------------------------------------------
void LHNode::removeSelf(){
    removeFromParentAndCleanup(true);
}
//------------------------------------------------------------------------------
LHNode::LHNode(){
}
//------------------------------------------------------------------------------
bool LHNode::isLHNode(CCNode* obj){
    
    if( 0 != dynamic_cast<LHNode*>(obj))
        return true;
    
    return false;
}
//------------------------------------------------------------------------------
LHNode* LHNode::nodeWithDictionary(LHDictionary* dictionary){
    LHNode *pobNode = new LHNode();
	if (pobNode && pobNode->initWithDictionary(dictionary))
    {
	    pobNode->autorelease();
        return pobNode;
    }
    CC_SAFE_DELETE(pobNode);
	return NULL;
}
//------------------------------------------------------------------------------
void LHNode::setBody(b2Body* b){
    body = b;
}
//------------------------------------------------------------------------------
b2Body* LHNode::getBody(void){
    return body;
}
//------------------------------------------------------------------------------
CCArray* LHNode::jointList(void){
    
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
#else
    CCArray* array = CCArray::array();
#endif
    
    if(body != NULL){
        b2JointEdge* jtList = body->GetJointList();
        while (jtList) {
            LHJoint* lhJt = LHJoint::jointFromBox2dJoint(jtList->joint);
            if(lhJt != NULL)
                array->addObject(lhJt);
            jtList = jtList->next;
        }
    }
    return array;
}
//------------------------------------------------------------------------------
bool LHNode::removeBodyFromWorld(){
    if(NULL != body){
		b2World* _world = body->GetWorld();
		if(0 != _world){
            CCArray* list = jointList();
            for(int i = 0; i < list->count(); ++i){
                LHJoint* jt = (LHJoint*)list->objectAtIndex(i);
                jt->setShouldDestroyJointOnDealloc(false);
                jt->removeSelf();
            }
            list->removeAllObjects();
            if(body)
                _world->DestroyBody(body);
			body = NULL;
            
            return true;
		}
	}
    return false;
}
//------------------------------------------------------------------------------
LHNode* LHNode::nodeForBody(b2Body* body){
    
    if(0 == body)
        return 0;
    
    CCNode* spr = (CCNode*)body->GetUserData();
    
    if(LHNode::isLHNode(spr))
    {
        return (LHNode*)spr;
    }
    
    return 0;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------