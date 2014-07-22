//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "FireBall.h"
#include "ViewPort.h"

#define FIRE_BALL_SPEED 10.0

FireBall::FireBall(uint32_t originPowerUpId, b2World* world)  {
    initWithSpriteFrameName("laser_purple.png");
    
    _originPowerUpId = originPowerUpId;
    
    _state = kFireBallPrepareToFire;
    
    // Build the Box2D body for the fire ball
    buildFireBallBody(world);
}

bool FireBall::isHit(CCNode* target) {
    // If the fire ball is not moving, then there is no hit
    if (_state != kFireBallMoving) return false;
    
    // If the target node is not available or not visible there will be no hit
    if (target == NULL || !target->isVisible()) return false;
    
    return target->boundingBox().intersectsRect(this->boundingBox());
}

void FireBall::buildFireBallBody(b2World* world) {
    CCPoint position = CCPointZero;
    
    //body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    
    b2Vec2 worldPosition = ViewPort::getInstance()->screenToWorldCoordinate(position);
    bodyDef.position.Set(worldPosition.x, worldPosition.y);
    
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.3, 0.3);
    
    //fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.0f; // So the fireball will be able to slide next to obstacles
    fixtureDef.restitution = 0.0f; // So the fireball will not bounce from the ground
    fixtureDef.filter.groupIndex = -1; //negative so the players will not collide
    
    //create dynamic body
    b2Body *body = world->CreateBody(&bodyDef);
    // body->SetBullet(true);
    body->SetGravityScale(0.0f); // Fire balls should have no gravity (go in straight line)
    
    //add main fixture
    body->CreateFixture(&fixtureDef);
    
    setB2Body(body);
    setPTMRatio(ViewPort::getInstance()->getPTMRatio());
    setPosition(position);
}
