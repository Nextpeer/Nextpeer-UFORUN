//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "ViewPort.h"
#include "cocos2d.h"
USING_NS_CC;

#include "LevelHelperLoader.h"

ViewPort::ViewPort() :
_screenWidth(0), _screenHeight(0), _screenStartXPosition(0)
{
}

ViewPort* ViewPort::_instance = 0;

ViewPort* ViewPort::getInstance()
{
    if (!_instance) _instance = new ViewPort();
    
    return _instance;
}

void ViewPort::initialize(CCSize designResolutionSize)
{
    _screenWidth = designResolutionSize.width;
    _screenHeight = designResolutionSize.height;
}

void ViewPort::setScreenStartXPosition(float screenStartXPosition)
{
    // screen movement is negative to the player movement, therefore the axis are different.
    _screenStartXPosition = -1 * (screenStartXPosition);
}

CCPoint ViewPort::worldToScreenCoordinate(b2Vec2 worldCoordinate)
{
    return LevelHelperLoader::metersToPoints(worldCoordinate);
}

b2Vec2 ViewPort::screenToWorldCoordinate(CCPoint screenCoordinate)
{
    return LevelHelperLoader::pointsToMeters(screenCoordinate);
}

bool ViewPort::isWorldCoordinateOnScreen(b2Vec2 worldCoordinate)
{
    CCPoint screenPosition = worldToScreenCoordinate(worldCoordinate);
    return isScreenCoordinateOnScreen(screenPosition);
}

bool ViewPort::isScreenCoordinateOnScreen(CCPoint screenPosition)
{
    float xStart = _screenStartXPosition;
    float xEnd = xStart + _screenWidth;
    
    return (screenPosition.x >= xStart && screenPosition.x <= xEnd);
}

bool ViewPort::isScreenElementOnScreen(CCPoint center, CCSize size)
{
    CCPoint screenBorderTopRight = center;
    CCPoint screenBorderBottomLeft = center;
    
    // It's enough to check the sprite's top right and bottom left corners
    screenBorderTopRight.x += size.width / 2.0f;
    screenBorderTopRight.y += size.height / 2.0f;
    
    screenBorderBottomLeft.x -= size.width / 2.0f;
    screenBorderBottomLeft.y -= size.height / 2.0f;
    
    return isScreenCoordinateOnScreen(screenBorderTopRight) ||
    isScreenCoordinateOnScreen(screenBorderBottomLeft);
}

float ViewPort::getPTMRatio() const
{
    return LevelHelperLoader::meterRatio();
}