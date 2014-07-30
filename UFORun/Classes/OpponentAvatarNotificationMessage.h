//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__OpponentAvatarNotificationMessage__
#define __UFORun__OpponentAvatarNotificationMessage__

#include "MultiplayerMessage.h"
#include "PlayerSpriteProfile.h"
#include "PlayerData.h"

typedef struct __attribute__ ((__packed__)) {
    MultiplayerMessageHeader header;
    uint32_t avatarIdentifier;
} OpponentAvatarNotificationMessageStruct;

/**
 Notify about the opponent's avatar selection.
 */
class OpponentAvatarNotificationMessage : public MultiplayerMessage
{
protected:
    bool extractDataFromByteVector(const vector<unsigned char>& byteVector);    
    
public:
    OpponentAvatarNotificationMessage();
    PlayerData* toPlayerData();

    static OpponentAvatarNotificationMessage* createWithP2PData(const TournamentP2PData& tournamentP2PData);
    static OpponentAvatarNotificationMessage* createWithPlayerData(const PlayerData& playerData);
    
    virtual vector<unsigned char>& toByteVector();
    
    virtual MultiplayerMessageType getMessageType();
    
public:
    CC_SYNTHESIZE(GamePlayerProfileType, _avatarIdentifier, AvatarIdentifier);
};


#endif /* defined(__UFORun__OpponentAvatarNotificationMessage__) */
