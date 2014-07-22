//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__ViewPort__
#define __UFORun__ViewPort__

#include "cocos2d.h"
#include "Box2D.h"

USING_NS_CC;

/**
 Helper class. Responsibles for Screen <-> World position translation. Scree/World related properties (PTM ratio, Screen's width/height).
 */
class ViewPort : public CCObject
{
    
public:
    ViewPort();
    
    static ViewPort* getInstance();
    
    void initialize(CCSize designResolutionSize);
    void setScreenStartXPosition(float screenStartXPosition);
    float getScreenStartXPosition() {
        return _screenStartXPosition;
    }
    
    CCPoint worldToScreenCoordinate(b2Vec2 worldCoordinate);
    b2Vec2 screenToWorldCoordinate(CCPoint screenCoordinate);
    
    bool isWorldCoordinateOnScreen(b2Vec2 worldCoordinate);
    bool isScreenCoordinateOnScreen(CCPoint screenPosition);
    bool isScreenElementOnScreen(CCPoint center, CCSize size);
    
    float getPTMRatio(void) const;
    
private:
    static ViewPort* _instance;
    float _screenStartXPosition;
    CC_SYNTHESIZE(float, _screenWidth, ScreenWidth);
    CC_SYNTHESIZE(float, _screenHeight, ScreenHeight);
};

#endif /* defined(__UFORun__ViewPort__) */
