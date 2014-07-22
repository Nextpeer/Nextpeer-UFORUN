//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__PlayerSpriteProfile__
#define __UFORun__PlayerSpriteProfile__

#include "cocos2d.h"
#include "Box2D.h"

USING_NS_CC;

/**
 Wraps the different characters assets to the an interface.
 */
typedef enum
{
    kGamePlayerProfileBlue = 0,
    kGamePlayerProfilePink,
    kGamePlayerProfileGreen,
    kGamePlayerProfileBeige,
    
    GamePlayerProfileTypeMax // max sentinal
    
} GamePlayerProfileType;

class GamePlayerProfile : public CCObject {
    public:
    virtual GamePlayerProfileType profileType() = 0; // The type of the player profile
    virtual const char* assetName() = 0; // Custom asset name
    virtual const char* spriteFrontName() = 0; // Sprite frame name for the front state
    virtual const char* spriteJumpName() = 0; // Sprite frame name for the jump state
    virtual const char* spriteRunName() = 0; // Sprite formatted %d frame name for the run animation
    virtual unsigned int spriteRunFrameCount() = 0; // Return the count of frames for the run animation
    virtual float spriteRunDelayPerFrameUnit() = 0; // Return the delay between each frame in the run animation
    virtual const char* spriteHurtName() = 0; // Sprite frame name for the hurt state
    virtual const char* spriteStandName() = 0; // Sprite frame name for the stand state
    virtual const char* spriteExplosionName(); // Sprite formatted %d frame name for the explosion animation
    virtual unsigned int spriteExplosionFrameCount(); // Return the count of frames for the explosion animation
    virtual float spriteExplosionDelayPerFrameUnit(); // Return the delay between each frame in the explosion animation
    virtual const char* spriteSpeedBoostName(); // Sprite formatted %d frame name for the speed lines animation
    virtual unsigned int spriteSpeedBoostFrameCount(); // Return the count of frames for the speed lines animation
    virtual float spriteSpeedBoostDelayPerFrameUnit(); // Return the delay between each frame in the speed lines animation

    
    virtual b2PolygonShape spriteBox2DShape() = 0; // The Box2D shape for this avatar.
};

class BluePlayerProfile : public GamePlayerProfile {
    public:
    virtual GamePlayerProfileType profileType();
    virtual const char* assetName();
    virtual const char* spriteFrontName();
    virtual const char* spriteJumpName();
    virtual const char* spriteRunName();
    virtual unsigned int spriteRunFrameCount();
    virtual float spriteRunDelayPerFrameUnit();
    virtual const char* spriteHurtName();
    virtual const char* spriteStandName();
    virtual b2PolygonShape spriteBox2DShape();
};

class PinkPlayerProfile : public GamePlayerProfile {
    public:
    virtual GamePlayerProfileType profileType();
    virtual const char* assetName();
    virtual const char* spriteFrontName();
    virtual const char* spriteJumpName();
    virtual const char* spriteRunName();
    virtual unsigned int spriteRunFrameCount();
    virtual float spriteRunDelayPerFrameUnit();
    virtual const char* spriteHurtName();
    virtual const char* spriteStandName();
    virtual b2PolygonShape spriteBox2DShape();
};

class GreenPlayerProfile : public GamePlayerProfile {
    public:
    virtual GamePlayerProfileType profileType();
    virtual const char* assetName();
    virtual const char* spriteFrontName();
    virtual const char* spriteJumpName();
    virtual const char* spriteRunName();
    virtual unsigned int spriteRunFrameCount();
    virtual float spriteRunDelayPerFrameUnit();
    virtual const char* spriteHurtName();
    virtual const char* spriteStandName();
    virtual b2PolygonShape spriteBox2DShape();
};


class BeigePlayerProfile : public GamePlayerProfile {
    public:
    virtual GamePlayerProfileType profileType();
    virtual const char* assetName();
    virtual const char* spriteFrontName();
    virtual const char* spriteJumpName();
    virtual const char* spriteRunName();
    virtual unsigned int spriteRunFrameCount();
    virtual float spriteRunDelayPerFrameUnit();
    virtual const char* spriteHurtName();
    virtual const char* spriteStandName();
    virtual b2PolygonShape spriteBox2DShape();
};

/**
 Factory for GamePlayerProfile by type.
 */
class GamePlayerProfileFactory {
    public:
    static GamePlayerProfile* createProfileByTypeOrNull(GamePlayerProfileType profileType);
    static CCArray* allGamePlayerProfiles();
};

/**
 Responsible for storing the current player's game profile in the device's application persistence storage.
 */
class CurrentPlayerGameProfile {
    public:
    static GamePlayerProfileType getCurrentUserProfile(); // Get the current user profile, or randomize one if does not exist
    static void setCurrentUserProfile(GamePlayerProfileType profileType);
};


#endif /* defined(__UFORun__PlayerSpriteProfile__) */
