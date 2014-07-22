//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "CCBox2DPhysicsRotateBy.h"
#include "Box2D.h"

CCBox2DPhysicsRotateBy* CCBox2DPhysicsRotateBy::create(float fDuration, float fDeltaAngle)
{
    CCBox2DPhysicsRotateBy *pRotateBy = new CCBox2DPhysicsRotateBy();
    pRotateBy->initWithDuration(fDuration, fDeltaAngle);
    pRotateBy->autorelease();
    
    return pRotateBy;
}

bool CCBox2DPhysicsRotateBy::initWithDuration(float fDuration, float fDeltaAngle)
{
    if (CCActionInterval::initWithDuration(fDuration))
    {
        m_fStartAngle = 0;
        m_fAngle = fDeltaAngle;
        _physicsSprite = NULL;
        return true;
    }
    
    return false;
}

CCObject* CCBox2DPhysicsRotateBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCBox2DPhysicsRotateBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCBox2DPhysicsRotateBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCBox2DPhysicsRotateBy();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone);
    
    pCopy->initWithDuration(m_fDuration, m_fAngle);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCBox2DPhysicsRotateBy::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    
    _physicsSprite = static_cast<CCPhysicsSprite *>(pTarget);
    
    if (_physicsSprite)
    {
        b2Body *body = _physicsSprite->getB2Body();
        m_fStartAngle = CC_RADIANS_TO_DEGREES(body->GetAngle());
    }
}

void CCBox2DPhysicsRotateBy::update(float time)
{
    if (_physicsSprite)
    {
        b2Body *body = _physicsSprite->getB2Body();
        
        b2Vec2 p = body->GetPosition();
        float radians = CC_DEGREES_TO_RADIANS(m_fStartAngle + m_fAngle * time);
        body->SetTransform(p, radians);
    }
}