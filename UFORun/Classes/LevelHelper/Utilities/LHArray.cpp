//
//  LHArray.cpp
//  LevelHelper API for Cocos2d-X
//
//  Created by Bogdan Vladu on 15.12.2011.
//  Copyright (c) 2011 Bogdan Vladu. All rights reserved.
//

#include "LHArray.h"
#include "LHObject.h"
//------------------------------------------------------------------------------
void LHArray::insertObjectsInVector(std::vector<std::string>* vec){
    for(int i = 0; i< count(); ++i)
    {
        LHObject* obj = objectAtIndex(i);
        if(obj){
            if(obj->type() == LHObject::STRING_TYPE){
                vec->push_back(obj->stringValue());
            }
        }    
    }
}
//------------------------------------------------------------------------------
void LHArray::insertObjectsInVector(std::vector<float>* vec){
    for(int i = 0; i< count(); ++i)
    {
        LHObject* obj = objectAtIndex(i);
        if(obj){
            vec->push_back(obj->floatValue());
        }
    }
}
//------------------------------------------------------------------------------
void LHArray::insertObjectsInVector(std::vector<bool>* vec){
    for(int i = 0; i< count(); ++i)
    {
        LHObject* obj = objectAtIndex(i);
        if(obj){
            vec->push_back(obj->boolValue());
        }
    }
}
//------------------------------------------------------------------------------