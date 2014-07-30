//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__Player__
#define __UFORun__Player__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Box2D.h"
#include "PlayerSpriteProfile.h"
#include "PlayerData.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define PLAYER_DEFAULT_LINEAR_SPEED_X 6.0f
#define HURT_BY_FIRE_BALL_TIMEOUT 3.0f

#define PLAYER_BONUS_PER_SECOND 60.0f
#define PLAYER_SCORE_DISTANCE_MODIFIER 10

/**
 The supported player effects, extend this enum if you wish to add more effects.
 */
typedef enum
{
    kPlayerEffectAnimationHurt = 0,
    kPlayerEffectAnimationShield,
    kPlayerEffectAnimationSpeedBoost
    
} PlayerEffectAnimation;

/**
 Abstract class. Responsible for the player rendering on screen.
 */
class Player : public CCPhysicsSprite {
    
private:
    PlayerData *_playerData;    // The player's multiplayer data (name, player id)
    GamePlayerProfile *_profile;   // The player's game profile (avatar, asset details)
    CCAction * _runAction; // The player run animation (animation frames)
    CCAction * _hurtAction; // The player hurt (explosion) animation (animation frames)
    CCAction * _speedBoostAction; // The player speed boost animation (animation frames)
    CCPhysicsSprite *_playerAnimationNode; // We will not show the animation effects on the player node, but on a overlay node.
    
    void buildPlayerBox2DBody(b2World* world);
    void addPlayerAnimationNode(b2World* world);
    
    void showHurtAnimation();
    void showShieldAnimation();
    
protected:
    void showRunAnimation();
    void showReadyAnimation();
    void showJumpAnimation();
    void showSpeedBoostAnimation();
    
    void showStoppedAnimation();
    
    void showPlayerEffectAnimation(PlayerEffectAnimation playerEffect);
    void stopPlayerEffectAnimation();
public:
	
	Player(GamePlayerProfile *profile, b2World* world, PlayerData *data);
	~Player();
    
	virtual void update (float dt) = 0;
    
    PlayerData* getPlayerData() const{
    	return _playerData;
	}
    
    GamePlayerProfile* getPlayerProfile() const{
    	return _profile;
	}
};
#endif /* defined(__UFORun__Player__) */
