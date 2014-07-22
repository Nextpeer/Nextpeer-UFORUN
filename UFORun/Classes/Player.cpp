//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "Player.h"
#include "ViewPort.h"
#include "CCBox2DPhysicsRotateBy.h"

Player::~Player(){
	
	CC_SAFE_RELEASE_NULL(_profile);
	CC_SAFE_RELEASE_NULL(_runAction);
    CC_SAFE_RELEASE_NULL(_hurtAction);
    CC_SAFE_RELEASE_NULL(_playerAnimationNode);
    CC_SAFE_RELEASE_NULL(_speedBoostAction);
}

Player::Player(GamePlayerProfile *profile, b2World* world) {
    initWithSpriteFrameName(profile->spriteStandName());
    
    _profile = profile;
    _profile->retain();
    
	this->setAnchorPoint(ccp(0.5f, 1.0f));
    
    // Set up the run animation for the player
	CCAnimation* runAnimation = CCAnimation::create();
	CCSpriteFrame *frame;
	unsigned int i;
    const char* spriteRunName = _profile->spriteRunName();
	for(i = 1; i <= _profile->spriteRunFrameCount(); i++) {
		char szName[100] = {0};
		sprintf(szName, spriteRunName, i);
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(szName);
		runAnimation->addSpriteFrame(frame);
	}
	
	runAnimation->setDelayPerUnit(_profile->spriteRunDelayPerFrameUnit());
	runAnimation->setRestoreOriginalFrame(true);
	runAnimation->setLoops(-1);
	_runAction = CCAnimate::create(runAnimation);
	_runAction->retain();
    
    // Set up the explosion animation for the player
	CCAnimation* explosionAnimation = CCAnimation::create();
    const char* spriteExplosionName = _profile->spriteExplosionName();
	for(i = 1; i <= _profile->spriteExplosionFrameCount(); i++) {
		char szName[100] = {0};
		sprintf(szName, spriteExplosionName, i);
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(szName);
		explosionAnimation->addSpriteFrame(frame);
	}
	
	explosionAnimation->setDelayPerUnit(_profile->spriteExplosionDelayPerFrameUnit());
	explosionAnimation->setLoops(1); // The explosion will run only one loop
    
    // Show the explosion, then hide the node wait for a delay time
	_hurtAction = CCSequence::create(CCShow::create(), CCAnimate::create(explosionAnimation), CCHide::create(), NULL);
	_hurtAction->retain();
    
    // Set up the speed boost animation for the player
	CCAnimation* speedBoostAnimation = CCAnimation::create();
    const char* spriteSpeedBoostName = _profile->spriteSpeedBoostName();
	for(i = 1; i <= _profile->spriteSpeedBoostFrameCount(); i++) {
		char szName[100] = {0};
		sprintf(szName, spriteSpeedBoostName, i);
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(szName);
		speedBoostAnimation->addSpriteFrame(frame);
	}
	
	speedBoostAnimation->setDelayPerUnit(_profile->spriteSpeedBoostDelayPerFrameUnit());
	speedBoostAnimation->setRestoreOriginalFrame(true);
	speedBoostAnimation->setLoops(-1);
    
	_speedBoostAction = CCAnimate::create(speedBoostAnimation);
	_speedBoostAction->retain();
    
    buildPlayerBox2DBody(world);
    addPlayerAnimationNode(world);
}

void Player::showRunAnimation() {
    this->stopAllActions();
    this->runAction(_runAction);
}

void Player::showReadyAnimation() {
    this->stopAllActions();
    setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_profile->spriteStandName()));
}

void Player::showJumpAnimation() {
    this->stopAllActions();
    setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_profile->spriteJumpName()));
}
void Player::showStoppedAnimation() {
    this->stopAllActions();
    setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_profile->spriteFrontName()));
}

void Player::buildPlayerBox2DBody(b2World* world) {
    CCPoint position = CCPointZero;
    
    //body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    
    b2Vec2 worldPosition = ViewPort::getInstance()->screenToWorldCoordinate(position);
    bodyDef.position.Set(worldPosition.x, worldPosition.y);
    
    //shape definition for main fixture
    b2PolygonShape dynamicBox = _profile->spriteBox2DShape();
    
    //fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.0f; // So the player will be able to slide next to obstacles
    fixtureDef.restitution = 0.0f; // So the player will not bounce again from the ground
    fixtureDef.filter.groupIndex = -1; //negative so the different players will not collide
    
    //create dynamic body
    b2Body *body = world->CreateBody(&bodyDef);
    
    //add main fixture
    body->CreateFixture(&fixtureDef);
    
    setB2Body(body);
    setPTMRatio(ViewPort::getInstance()->getPTMRatio());
    setPosition(position);
}

// In order to show the animation on top of the player it is required to add another node on top of the player node (child).
// This node will run the explosion animation.
void Player::addPlayerAnimationNode(b2World* world) {
    
    _playerAnimationNode = CCPhysicsSprite::create();
    _playerAnimationNode->retain();
    _playerAnimationNode->setContentSize(getContentSize());
    
    // As the this sprite is actually a physics sprite, we need to make the animation node as a static body
    b2BodyDef playerAnimationNodeBodyDef;
    playerAnimationNodeBodyDef.type = b2_staticBody;
    
    b2Body *playerAnimationNodeBody = world->CreateBody(&playerAnimationNodeBodyDef);
    playerAnimationNodeBody->SetFixedRotation(true);
    
    _playerAnimationNode->setB2Body(playerAnimationNodeBody);
    _playerAnimationNode->setPTMRatio(ViewPort::getInstance()->getPTMRatio());
    
    // Place above the player
    _playerAnimationNode->setPosition(ccp(this->getContentSize().width / 2, this->getContentSize().height/ 2));
    
    addChild(_playerAnimationNode);
}

void Player::showPlayerEffectAnimation(PlayerEffectAnimation playerEffect) {
    
    switch (playerEffect) {
        case kPlayerEffectAnimationHurt:
            showHurtAnimation();
            break;
        case kPlayerEffectAnimationShield:
            showShieldAnimation();
            break;
            
        case kPlayerEffectAnimationSpeedBoost:
            showSpeedBoostAnimation();
            break;
            
        default:
            break;
    }
}

void Player::stopPlayerEffectAnimation() {
    _playerAnimationNode->stopAllActions();
    _playerAnimationNode->setRotation(0);
    _playerAnimationNode->setVisible(false); // Make sure to run CCHide.
}

void Player::showHurtAnimation() {
    
    this->stopAllActions();
    // Set the display frame as long as the player is running
    setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_profile->spriteHurtName()));
    
    _playerAnimationNode->stopAllActions();
    _playerAnimationNode->setVisible(true); // Make sure the node is visible
    _playerAnimationNode->runAction(_hurtAction);
}

void Player::showShieldAnimation() {
    
    _playerAnimationNode->stopAllActions();
    _playerAnimationNode->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("shield.png"));
    _playerAnimationNode->setVisible(true); // Make sure the node is visible
    _playerAnimationNode->runAction(CCRepeatForever::create(CCBox2DPhysicsRotateBy::create(0.1, 10)));
}

void Player::showSpeedBoostAnimation() {
    _playerAnimationNode->stopAllActions();
    _playerAnimationNode->setVisible(true); // Make sure the node is visible
    _playerAnimationNode->runAction(_speedBoostAction);
}
