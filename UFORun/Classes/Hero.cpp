//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "Hero.h"
#include "ViewPort.h"
#include "CCNextpeer.h"

#define PLAYER_JUMP_Y_SPEED_WHEN_STUCK 8.0f
#define PLAYER_JUMP_Y_SPEED_WHILE_RUN 6.0f
#define MIN_DISTANCE_DELTA_THRESHOLD_FOR_STUCK 1.0f
#define SHIELD_POWER_UP_TIMEOUT 4.0f
#define SPEED_BOOST_POWER_UP_TIMEOUT 3.5f
#define SPEED_BOOST_POWER_UP_INCREASE 2.0f

Hero::Hero(GamePlayerProfile *profile, b2World* world, PlayerData *data) : Player(profile, world, data)
{
    // Record the position;
    _speedX = PLAYER_DEFAULT_LINEAR_SPEED_X;
    _canJump = true;
	_state = kHeroReady;
    _isStuckOnScreen = false;
    _isHurt = false;
    _heroLastKnownScreenXPosition = 0;
    _powerUpState = kHeroPowerUpStateNone;
    
    addCurrentUserMarker();
    addFootSensor();
}

Hero* Hero::create(b2World* world)
{
    // Create the Hero player data
    string playerName = nextpeer::CCNextpeer::getInstance()->getCurrentPlayerName();
    string playerId = nextpeer::CCNextpeer::getInstance()->getCurrentPlayerId();
    
    PlayerData *heroData = PlayerData::create(playerId, playerName, false);
    heroData->setProfileType(CurrentPlayerGameProfile::getCurrentUserProfile());
    GamePlayerProfile *profile = GamePlayerProfileFactory::createProfileByTypeOrNull(heroData->getProfileType());
    
    Hero *hero = new Hero(profile, world, heroData);
    
	if (hero != NULL) {
		hero->autorelease();
        return hero;
	}
	CC_SAFE_DELETE(hero);
	return NULL;
}

void Hero::addCurrentUserMarker()
{
    CCPhysicsSprite* marker = CCPhysicsSprite::createWithSpriteFrameName("current_user_mark.png");
    
    b2World *world = getB2Body()->GetWorld();
    
    ViewPort *viewPort = ViewPort::getInstance();
    
    // As the this sprite is actually a physics sprite, we need to make the user mark as a static body
    b2BodyDef markerBodyDef;
    markerBodyDef.type = b2_staticBody;
    CCPoint position = getPosition();
    
    b2Vec2 worldPosition = viewPort->screenToWorldCoordinate(position);
    markerBodyDef.position.Set(worldPosition.x, worldPosition.y);
    
    b2Body *markerBody = world->CreateBody(&markerBodyDef);
    markerBody->SetFixedRotation(true);
    // Define the static body fixture.
    marker->setB2Body(markerBody);
    marker->setPTMRatio(viewPort->getPTMRatio());
    
    // Place above the player
    marker->setPosition(ccp(this->getContentSize().width / 2,
                            this->getContentSize().height + marker->getContentSize().height));
    
    this->addChild(marker);
}

void Hero::addFootSensor() {
    
    // Add the foot sensor fixture
    b2PolygonShape dynamicBox;
    b2FixtureDef fixtureDef;
    
    dynamicBox.SetAsBox(0.3, 0.3, b2Vec2(0,-2), 0);
    
    //fixture definition
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1;
    
    fixtureDef.isSensor = true;
    b2Fixture* footSensorFixture = getB2Body()->CreateFixture(&fixtureDef);
    footSensorFixture->SetUserData( (void*)HERO_FOOT);
}

void Hero::update (float dt)
{
    // If the hero is running
    if (_state == kHeroRunning) {
        
        // Keep moving the hero at a constant X speed
        b2Body* playerBody = getB2Body();
        b2Vec2 currentLinearVel = playerBody->GetLinearVelocity();
        currentLinearVel.x = _speedX;
        playerBody->SetLinearVelocity(currentLinearVel);
        
        // Record the last position, check if there is any significant movement
        _isStuckOnScreen = (abs(_heroLastKnownScreenXPosition - getPositionX()) <= MIN_DISTANCE_DELTA_THRESHOLD_FOR_STUCK);
        _heroLastKnownScreenXPosition = getPositionX();
    }
    // Else, if the hero passed the finish line
    else if (_state == kHeroPassedFinishLine) {
        
        // Make sure the body will not move (wait for the jump to be over).
        if (_canJump) {
            
            b2Body* playerBody = getB2Body();
            b2Vec2 currentLinearVel = playerBody->GetLinearVelocity();
            if (currentLinearVel.x < 1) {
                playerBody->SetLinearVelocity(b2Vec2(0,0));
                playerBody->SetAngularVelocity(0);
                
                // Show the stopped animation and switch the state
                showStoppedAnimation();
                _state = kHeroStopped;
            }
            else {
                currentLinearVel.x = currentLinearVel.x-0.1; // gradually decrease the speed till stop
                playerBody->SetLinearVelocity(currentLinearVel);
            }
        }
    }
}

bool Hero::getIsJumping() const {
    
    // If the player can jump -> that means the player is on the groun ->  can't jump
    return !_canJump;
}

void Hero::startRace() {
    move();
}

void Hero::jump () {
    // If the hero can jump apply the jump (not hurt, still running and can jump)
    if (!_isHurt && _state == kHeroRunning && (_canJump || _isStuckOnScreen)) {
        
        b2Body* playerBody = getB2Body();
        float jumpSpeed = (_isStuckOnScreen? PLAYER_JUMP_Y_SPEED_WHEN_STUCK:PLAYER_JUMP_Y_SPEED_WHILE_RUN);
        float impulse = playerBody->GetMass() * jumpSpeed;
        playerBody->SetLinearVelocity(b2Vec2(playerBody->GetLinearVelocity().x, 0));
        playerBody->ApplyLinearImpulse( b2Vec2(0,impulse), playerBody->GetWorldCenter() );
    }
}

void Hero::passedFinishLine() {
    // If the hero is still running mark the state as finish line, ignore otherwise.
    if (_state == kHeroRunning) {
        _state = kHeroPassedFinishLine;
    }
}

void Hero::move() {
    // Can switch back to move as long as the hero didn't reach the finish line
    if (_state >= kHeroPassedFinishLine) return;
    
    // Cannot run while hurt
    if (_isHurt) return;
    
    // Change the state of the player (can happen if the player is not jumping any more or on start of the game scene (uknown -> moving)
    _state = kHeroRunning;
    
    showRunAnimation();
}

void Hero::updateJumpability(bool canJump) {
    if (_canJump == canJump) {
        return;
    }
    
    // Switching state, make sure the animation has changed (jump<->move)
    _canJump = canJump;
    if (_canJump) {
        move();
    }
    else {
        showJumpAnimation();
    }
}

void Hero::useSpeedBoostPowerUp() {
    // Can't use power up while hurt
    if (_isHurt ) return;
    // Can't use power while one is already under use
    if (_powerUpState != kHeroPowerUpStateNone) return;
    _powerUpState = kHeroPowerUpStateSpeedBoost;
    
    _speedX += SPEED_BOOST_POWER_UP_INCREASE;
    showPlayerEffectAnimation(kPlayerEffectAnimationSpeedBoost);
    
    this->scheduleOnce(schedule_selector(Hero::powerUpUsageFinished), SPEED_BOOST_POWER_UP_TIMEOUT);
}

void Hero::useShieldPowerUp() {
    // Can't use power up while hurt
    if (_isHurt ) return;
    
    // Can't use power while one is already under use
    if (_powerUpState != kHeroPowerUpStateNone) return;
    _powerUpState = kHeroPowerUpStateShield;
    
    showPlayerEffectAnimation(kPlayerEffectAnimationShield);
    
    this->scheduleOnce(schedule_selector(Hero::powerUpUsageFinished), SHIELD_POWER_UP_TIMEOUT);
}

void Hero::powerUpUsageFinished() {
    stopPlayerEffectAnimation();
    _speedX = PLAYER_DEFAULT_LINEAR_SPEED_X; // restore speed
    _powerUpState = kHeroPowerUpStateNone;
}

void Hero::hitByFireBall() {
    // Can't hit the hero twice
    if (_isHurt ) return;
    
    _isHurt = true;
    _speedX = 0;
    showPlayerEffectAnimation(kPlayerEffectAnimationHurt);
    
    this->scheduleOnce(schedule_selector(Hero::hurtAnimationFinished), HURT_BY_FIRE_BALL_TIMEOUT);
}

void Hero::hurtAnimationFinished() {
    stopPlayerEffectAnimation();
    _isHurt = false;
    _speedX = PLAYER_DEFAULT_LINEAR_SPEED_X;
    
    // Restore the animation
    if (_state == kHeroRunning) {
        move();
    }
    else if (_state == kHeroStopped) {
        showStoppedAnimation();
    }
}
