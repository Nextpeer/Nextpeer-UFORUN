//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__CCSortableArray__
#define __UFORun__CCSortableArray__

#include "cocos2d.h"
USING_NS_CC;

typedef bool (*CCSortableComparator)(CCObject*,CCObject*);

/**
 Defines a CCArray that can be sorted.
 */
class CCSortableArray : public CCArray
{
public:
    static CCSortableArray* createWithArray(CCArray* array);
    
    void sort(CCSortableComparator comp);
};

#endif /* defined(__UFORun__CCSortableArray__) */
