//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "CCSortableArray.h"
#include <algorithm>

CCSortableArray* CCSortableArray::createWithArray(CCArray *array)
{
    CCSortableArray* sortable = new CCSortableArray();
    sortable->initWithArray(array);
    
    sortable->autorelease();
    
    return sortable;
}


void CCSortableArray::sort(CCSortableComparator comp)
{
    std::sort(data->arr, data->arr + data->num, comp);
}