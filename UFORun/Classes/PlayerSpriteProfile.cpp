//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "PlayerSpriteProfile.h"

// GamePlayerProfile
const char* GamePlayerProfile::spriteExplosionName() {
    return "explosion_%02d.png";
}

unsigned int GamePlayerProfile::spriteExplosionFrameCount() {
    return 9;
}

float GamePlayerProfile::spriteExplosionDelayPerFrameUnit() {
    return 0.2f/6.0f;
}

const char* GamePlayerProfile::spriteSpeedBoostName() {
    return "speed_lines_%02d.png";
}

unsigned int GamePlayerProfile::spriteSpeedBoostFrameCount() {
    return 13;
}

float GamePlayerProfile::spriteSpeedBoostDelayPerFrameUnit() {
    return 0.2f/8.0f;
}

// BluePlayerProfile
GamePlayerProfileType BluePlayerProfile::profileType() {
    return kGamePlayerProfileBlue;
}

const char* BluePlayerProfile::assetName() {
    return "Blue";
}

const char* BluePlayerProfile::spriteFrontName() {
    return "p2_front.png";
}

const char* BluePlayerProfile::spriteJumpName() {
    return "p2_jump.png";
}

const char* BluePlayerProfile::spriteRunName() {
    return "p2_walk%02d.png";
}

unsigned int BluePlayerProfile::spriteRunFrameCount() {
    return 11;
}

float BluePlayerProfile::spriteRunDelayPerFrameUnit() {
    return 0.2f/3.0f;
}

const char* BluePlayerProfile::spriteHurtName() {
    return "p2_hurt.png";
}

const char* BluePlayerProfile::spriteStandName() {
    return "p2_stand.png";
}

b2PolygonShape BluePlayerProfile::spriteBox2DShape() {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.6f,0.9f,b2Vec2(0.0f,-0.5F), 0.0F);
    return dynamicBox;
}

// PinkPlayerProfile
GamePlayerProfileType PinkPlayerProfile::profileType() {
    return kGamePlayerProfilePink;
}

const char* PinkPlayerProfile::assetName() {
    return "Pinky";
}

const char* PinkPlayerProfile::spriteFrontName() {
    return "p3_front.png";
}

const char* PinkPlayerProfile::spriteJumpName() {
    return "p3_jump.png";
}

const char* PinkPlayerProfile::spriteRunName() {
    return "p3_walk%02d.png";
}

unsigned int PinkPlayerProfile::spriteRunFrameCount() {
    return 11;
}

float PinkPlayerProfile::spriteRunDelayPerFrameUnit() {
    return 0.2f/3.0f;
}

const char* PinkPlayerProfile::spriteHurtName() {
    return "p3_hurt.png";
}

const char* PinkPlayerProfile::spriteStandName() {
    return "p3_stand.png";
}

b2PolygonShape PinkPlayerProfile::spriteBox2DShape() {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.6f,0.9f,b2Vec2(0.0f,-0.5F), 0.0F);
    return dynamicBox;
}

// GreenPlayerProfile
GamePlayerProfileType GreenPlayerProfile::profileType() {
    return kGamePlayerProfileGreen;
}

const char* GreenPlayerProfile::assetName() {
    return "Greeny";
}

const char* GreenPlayerProfile::spriteFrontName() {
    return "p1_front.png";
}

const char* GreenPlayerProfile::spriteJumpName() {
    return "p1_jump.png";
}

const char* GreenPlayerProfile::spriteRunName() {
    return "p1_walk%02d.png";
}

unsigned int GreenPlayerProfile::spriteRunFrameCount() {
    return 11;
}

float GreenPlayerProfile::spriteRunDelayPerFrameUnit() {
    return 0.2f/3.0f;
}

const char* GreenPlayerProfile::spriteHurtName() {
    return "p1_hurt.png";
}

const char* GreenPlayerProfile::spriteStandName() {
    return "p1_stand.png";
}

b2PolygonShape GreenPlayerProfile::spriteBox2DShape() {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.6f,0.9f,b2Vec2(0.0f,-0.5F), 0.0F);
    return dynamicBox;
}

// BeigePlayerProfile
GamePlayerProfileType BeigePlayerProfile::profileType() {
    return kGamePlayerProfileBeige;
}

const char* BeigePlayerProfile::assetName() {
    return "Dr. Beige";
}

const char* BeigePlayerProfile::spriteFrontName() {
    return "p4_front.png";
}

const char* BeigePlayerProfile::spriteJumpName() {
    return "p4_jump.png";
}

const char* BeigePlayerProfile::spriteRunName() {
    return "p4_walk%02d.png";
}

unsigned int BeigePlayerProfile::spriteRunFrameCount() {
    return 2;
}

float BeigePlayerProfile::spriteRunDelayPerFrameUnit() {
    return 0.15f;
}

const char* BeigePlayerProfile::spriteHurtName() {
    return "p4_hurt.png";
}

const char* BeigePlayerProfile::spriteStandName() {
    return "p4_stand.png";
}

b2PolygonShape BeigePlayerProfile::spriteBox2DShape() {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.6f,0.9f,b2Vec2(0.0f,-0.5F), 0.0F);
    return dynamicBox;
}

// GamePlayerProfileFactory
GamePlayerProfile* GamePlayerProfileFactory::createProfileByTypeOrNull(GamePlayerProfileType profileType) {
    GamePlayerProfile *profile = NULL;
    
    switch (profileType) {
        case kGamePlayerProfileGreen:
        profile = new GreenPlayerProfile();
        break;
        case kGamePlayerProfilePink:
        profile = new PinkPlayerProfile();
        break;
        case kGamePlayerProfileBlue:
        profile = new BluePlayerProfile();
        break;
        case kGamePlayerProfileBeige:
        profile = new BeigePlayerProfile();
        break;
        
        default:
        break;
    }
    
    if (profile != NULL) {
        profile->autorelease();
        return profile;
    }
    
    return NULL;
}

CCArray* GamePlayerProfileFactory::allGamePlayerProfiles() {
    CCArray* profiles = CCArray::create();
    profiles->addObject(createProfileByTypeOrNull(kGamePlayerProfileGreen));
    profiles->addObject(createProfileByTypeOrNull(kGamePlayerProfilePink));
    profiles->addObject(createProfileByTypeOrNull(kGamePlayerProfileBlue));
    profiles->addObject(createProfileByTypeOrNull(kGamePlayerProfileBeige));
    return profiles;
}

// CurrentPlayerGameProfile
#define KEY_OF_GAME_USER_PROFILE   "game-player-profile"
#define GAME_USER_PROFILE_NOT_FOUND   -999
GamePlayerProfileType CurrentPlayerGameProfile::getCurrentUserProfile() {
    
    int profileTypeAsInt = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_OF_GAME_USER_PROFILE, GAME_USER_PROFILE_NOT_FOUND);
    
    GamePlayerProfileType profileType;
    
    // Profile was not found -> randomize one & persist it.
    if (profileTypeAsInt == GAME_USER_PROFILE_NOT_FOUND) {
        profileTypeAsInt = random()%GamePlayerProfileTypeMax;
        profileType = static_cast<GamePlayerProfileType>(profileTypeAsInt);
        setCurrentUserProfile(profileType);
    }
    else {
        profileType = static_cast<GamePlayerProfileType>(profileTypeAsInt);
    }
    
    return profileType;
}

void CurrentPlayerGameProfile::setCurrentUserProfile(GamePlayerProfileType profileType) {
    CCUserDefault *userDefaults = CCUserDefault::sharedUserDefault();
    userDefaults->setIntegerForKey(KEY_OF_GAME_USER_PROFILE, profileType);
    userDefaults->flush();
}
