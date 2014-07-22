//
//  LHDictionary.h
//  LevelHelper API for Cocos2d-X
//
//  Created by Bogdan Vladu on 15.12.2011.
//  Copyright (c) 2011 Bogdan Vladu. All rights reserved.
//

#ifndef __LH_DICTIONARY_TYPE__
#define __LH_DICTIONARY_TYPE__


#include "cocos2d.h"
using namespace cocos2d;

class LHObject;
class LHArray;


#if COCOS2D_VERSION >= 0x00020000
class LHDictionary : public CCDictionary
#else
#include "cocoa/CCNS.h"
class LHDictionary : public CCDictionary<std::string, CCObject*>
#endif
{
  
public:
    
    LHDictionary* dictForKey(const std::string& key){
        return (LHDictionary*)this->objectForKey(key);
    }
    
    LHArray* arrayForKey(const std::string& key){
        return (LHArray*)this->objectForKey(key);
    }
    
    LHObject* objectForKey(const std::string& key){
        return (LHObject*)((CCDictionary*)this)->objectForKey(key);
    }
    
    
#if COCOS2D_VERSION >= 0x00020000
    
    void setObjectForKey(const std::string& obj, const std::string& key){
        this->setObject(new CCString(obj), key);
    }
    
    std::string stringForKey(const std::string& key){
        return std::string(this->valueForKey(key)->getCString());
    }
    
    float floatForKey(const std::string& key){
        return this->valueForKey(key)->floatValue();        
    }
    
    CCPoint pointForKey(const std::string& key){
        CCString* obj = (CCString*)this->objectForKey(key);
        return CCPointFromString(obj->getCString());
    }
    
    cocos2d::ccColor3B  colorForKey(const std::string& key){
        CCRect rect = CCRectFromString(this->valueForKey(key)->getCString());
        return ccc3(rect.origin.x*255.0f, rect.origin.y*255.0f, rect.size.width*255.0f);
    }
    
    CCSize sizeForKey(const std::string& key){
        return CCSizeFromString(this->valueForKey(key)->getCString());
    }
    
    CCRect rectForKey(const std::string& key){
        return CCRectFromString(this->valueForKey(key)->getCString());
    }
    
    bool boolForKey(const std::string& key){
        return this->valueForKey(key)->boolValue();
    }
    
    int intForKey(const std::string& key){
        return this->valueForKey(key)->intValue();
    }
    
#else

    
    void setObjectForKey(const std::string& obj, const std::string& key){
        this->setObject(new CCString(obj.c_str()), key);
    }
    
    std::string stringForKey(const std::string& key){
        return ((CCString*)this->objectForKey(key))->toStdString();
    }
    
    float floatForKey(const std::string& key){
        return ((CCString*)this->objectForKey(key))->toFloat();
    }
    
    CCPoint pointForKey(const std::string& key){
        CCString* obj = (CCString*)this->objectForKey(key);
        return CCPointFromString(obj->toStdString().c_str());
    }
    
    cocos2d::ccColor3B  colorForKey(const std::string& key){
        CCString* obj = (CCString*)this->objectForKey(key);
        CCRect rect = CCRectFromString(obj->toStdString().c_str());
        return ccc3(rect.origin.x*255.0f, rect.origin.y*255.0f, rect.size.width*255.0f);
    }
    
    CCSize sizeForKey(const std::string& key){
        CCString* obj = (CCString*)this->objectForKey(key);
        return CCSizeFromString(obj->toStdString().c_str());
    }
    
    CCRect rectForKey(const std::string& key){
        CCString* obj = (CCString*)this->objectForKey(key);
        return CCRectFromString(obj->toStdString().c_str());
    }
    
    bool boolForKey(const std::string& key){
        return (bool)((CCString*)this->objectForKey(key))->toInt();
    }
    
    int intForKey(const std::string& key){
        return ((CCString*)this->objectForKey(key))->toInt();
    }
    
#endif
    
   
};
#endif
