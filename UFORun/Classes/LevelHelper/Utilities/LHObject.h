//
//  LHObject.h
//  LevelHelper API for Cocos2d-X
//
//  Created by Bogdan Vladu on 15.12.2011.
//  Copyright (c) 2011 Bogdan Vladu. All rights reserved.
//

#ifndef __LH_OBJECT_TYPE__
#define __LH_OBJECT_TYPE__

#include "cocos2d.h"
using namespace std;
using namespace cocos2d;

class LHDictionary;
class LHArray;
class LHBatch;

class LHObject : public CCObject
{
public:
    
    LHArray* arrayValue(){
        return (LHArray*)this;
    }
    
    enum OBJECT_TYPE
    {
        INT_TYPE    = 0,
        FLOAT_TYPE  = 1,
        BOOL_TYPE   = 2,
        STRING_TYPE = 3,
        LH_DICT_TYPE = 4,
        LH_ARRAY_TYPE = 5,
        LH_VOID_TYPE = 6
    };
    
    OBJECT_TYPE type(void)
    {
        if( 0 != dynamic_cast<CCString*>(this))
            return STRING_TYPE;
        
        if( 0 != dynamic_cast<CCArray*>(this))
            return LH_ARRAY_TYPE;
    
#if COCOS2D_VERSION >= 0x00020000
        if( 0 != dynamic_cast<CCDictionary*>(this))
            return LH_DICT_TYPE;
#else
        if( 0 != dynamic_cast< CCDictionary<std::string, CCObject*>* >(this))
            return LH_DICT_TYPE;
#endif

        return LH_VOID_TYPE;
    }

#if COCOS2D_VERSION >= 0x00020000

    std::string stringValue(){
        return std::string(((CCString*)this)->getCString());
    }
    
    float floatValue(){
        return ((CCString*)this)->floatValue();
    }
    int intValue(){
        return ((CCString*)this)->intValue();
    }
    bool boolValue(){
        return ((CCString*)this)->boolValue();
    }
#else
    
    std::string stringValue(){
        return ((CCString*)this)->toStdString();
    }
    
    float floatValue(){
        return ((CCString*)this)->toFloat();
    }
    int intValue(){
        return ((CCString*)this)->toInt();
    }
    bool boolValue(){
        return (bool)((CCString*)this)->toInt();
    }
    
#endif
    
    
};

#endif
