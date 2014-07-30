//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__OpponentUpdate__
#define __UFORun__OpponentUpdate__

#include "MultiplayerMessage.h"
#include "MessageParser.h"
#include "Hero.h"
#include "Opponent.h"
#include "NextpeerNotifications.h"
using namespace nextpeer;


typedef struct __attribute__ ((__packed__)) {
    MultiplayerMessageHeader header;
    
    float worldPositionX; // The world X position of the opponent
    float worldPositionY; // The world Y position of the opponent
    
    // Forces that apply at the time of the update
    float linearVelocityX; // The linear velocity on the X axe of the opponent
    float linearVelocityY; // The linear velocity on the Y axe of the opponen
    
    bool isStuck; // Does the opponent stuck in an obstacle
    bool isHurt; // Does the opponent showing an hurt animation - got hit by a fireball
    uint32_t state; // The current state of the opponent (Running/Stopped)
    uint32_t powerUpState; // The current state of the opponent’s power up, does the shield turned on?
    
} OpponentUpdateMessageStruct;

/**
 The last known update from an opponent, from state to location.
 */
class OpponentUpdate : public MultiplayerMessage
{
protected:
    bool extractDataFromByteVector(const vector<unsigned char>& byteVector);
    
public:
    // Creation
    static OpponentUpdate* createWithP2PData(const TournamentP2PData& tournamentP2PData);
    static OpponentUpdate* createWithHero(const Hero* hero);

    virtual vector<unsigned char>& toByteVector();
    virtual MultiplayerMessageType getMessageType();
    
    OpponentUpdate();
    
    CC_SYNTHESIZE_READONLY(float, _worldPositionX, WorldPositionX);
    CC_SYNTHESIZE_READONLY(float, _worldPositionY, WorldPositionY);
    CC_SYNTHESIZE_READONLY(float, _linearVelocityX, LinearVelocityX);
    CC_SYNTHESIZE_READONLY(float, _linearVelocityY, LinearVelocityY);
    CC_SYNTHESIZE_READONLY(bool, _isStuck, IsStuck);
    CC_SYNTHESIZE_READONLY(bool, _isHurt, IsHurt);
    CC_SYNTHESIZE_READONLY(OpponentState, _state, State);
    CC_SYNTHESIZE_READONLY(OpponentPowerUpState, mPowerUpState, PowerUpState);
};

#endif /* defined(__UFORun__OpponentUpdate__) */
