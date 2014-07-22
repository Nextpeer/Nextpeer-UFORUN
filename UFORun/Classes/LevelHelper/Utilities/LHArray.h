//
//  LHArray.h
//  LevelHelper API for Cocos2d-X
//
//  Created by Bogdan Vladu on 15.12.2011.
//  Copyright (c) 2011 Bogdan Vladu. All rights reserved.
//

#ifndef __LH_ARRAY_TYPE__
#define __LH_ARRAY_TYPE__

#include "cocos2d.h"
using namespace cocos2d;


#if COCOS2D_VERSION >= 0x00020000
#else
#include "cocoa/CCNS.h"
#endif


class LHObject;
class LHDictionary;
class LHArray : public CCArray
{
public:
    
    LHDictionary* dictAtIndex(unsigned int index)
    {
        return (LHDictionary*)this->objectAtIndex(index);
    }
    
    
    LHArray* arrayAtIndex(unsigned int index)
    {
        return (LHArray*)this->objectAtIndex(index);
    }
    
    LHObject* objectAtIndex(unsigned int index)
    {
        return (LHObject*)((CCArray*)this)->objectAtIndex(index);
    }
    
    
#if COCOS2D_VERSION >= 0x00020000
    CCPoint pointAtIndex(unsigned int index){
        return CCPointFromString(((CCString*)this->objectAtIndex(index))->getCString());
    }
#else
    CCPoint pointAtIndex(unsigned int index){
        CCString* obj = (CCString*)this->objectAtIndex(index);
        return CCPointFromString(obj->toStdString().c_str());
    }
#endif
    
    
    void insertObjectsInVector(std::vector<std::string>* vec);
    void insertObjectsInVector(std::vector<float>* vec);
    void insertObjectsInVector(std::vector<bool>* vec);
};

#endif
