//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__OpponentFireBallUpdateMessage__
#define __UFORun__OpponentFireBallUpdateMessage__

#include "MultiplayerMessage.h"
#include "MessageParser.h"
#include "HeroFireBall.h"

typedef struct __attribute__ ((__packed__)) {
    MultiplayerMessageHeader header;
    
    uint32_t originPowerUpId; // The power up box that this fire ball came from. Power up boxes number with sequence 1..X (X number of power ups in the level)
    float worldPositionX; // The world X position of the fire ball
    float worldPositionY; // The world Y position of the fire ball
} OpponentFireBallUpdateMessageStruct;

/**
 The last known update from an opponent fire ball.
 */
class OpponentFireBallUpdateMessage : public MultiplayerMessage
{
protected:
    bool extractDataFromByteVector(const vector<unsigned char>& byteVector);
    
public:
    OpponentFireBallUpdateMessage();
    
    static OpponentFireBallUpdateMessage* createWithP2PData(const TournamentP2PData& tournamentP2PData);
    static OpponentFireBallUpdateMessage* createWithFireBall(const HeroFireBall* fireBall);
    
    virtual vector<unsigned char>& toByteVector();
    virtual MultiplayerMessageType getMessageType();
    
    CC_SYNTHESIZE_READONLY(float, _worldPositionX, WorldPositionX);
    CC_SYNTHESIZE_READONLY(float, _worldPositionY, WorldPositionY);
    CC_SYNTHESIZE_READONLY(uint32_t, _originPowerUpId, OriginPowerUpId);
};

#endif /* defined(__UFORun__OpponentFireBallUpdateMessage__) */