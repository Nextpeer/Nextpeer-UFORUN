//
//  LHNode.h
//  
//
//  Created by Bogdan Vladu on 4/2/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//

#ifndef __LHNODE_NODE__
#define __LHNODE_NODE__


#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class LHDictionary;
class LHNode : public CCNode
{
public:
    virtual bool initWithDictionary(LHDictionary* dictionary);
    virtual ~LHNode();
    LHNode();
    
    void removeSelf();
    
    static LHNode* nodeWithDictionary(LHDictionary* dictionary);
    
    void setBody(b2Body* body);
    b2Body* getBody(void);
    
    std::string& getUniqueName(){return uniqueName;}
    
    static bool isLHNode(CCNode* obj);
        
    //--------------------------------------------------------------------------
    std::string uniqueName;
    b2Body* body;

    static LHNode*  nodeForBody(b2Body* body);
    
    CCArray* jointList(void);
    bool removeBodyFromWorld(void);
    
private:

    friend class LH_b2DestructionListener;

    void nullifyBody(){body = NULL;} //called when LH_b2DestructionListener destroyes a fixture
};

#endif
