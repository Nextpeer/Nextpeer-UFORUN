//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__CCBox2DPhysicsRotateBy__
#define __UFORun__CCBox2DPhysicsRotateBy__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCPhysicsSprite.h"
USING_NS_CC;
USING_NS_CC_EXT;

/** @brief Rotates a CCPhysicsSprite object clockwise a number of degrees by modifying it's rotation attribute.
 *  @note CCRotateBy has a bug -> as it calls setRotateX/setRotateY which aren't override by CCPhysicsSprite.
 *  At this action we modify the Box2D body directly (hence will not work on Chipmunk engine).
 */
class CCBox2DPhysicsRotateBy : public CCActionInterval
{
public:
    /** creates the action */
    static CCBox2DPhysicsRotateBy* create(float fDuration, float fDeltaAngle);
    /** initializes the action */
    bool initWithDuration(float fDuration, float fDeltaAngle);
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float time);
    
protected:
    CCPhysicsSprite *_physicsSprite;
    float m_fAngle;
    float m_fStartAngle;
};


#endif /* defined(__UFORun__CCBox2DPhysicsRotateBy__) */
