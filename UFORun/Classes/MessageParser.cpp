//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "MessageParser.h"

#include "OpponentUpdate.h"
#include "OpponentFireBallUpdateMessage.h"
#include "OpponentAvatarNotificationMessage.h"

MultiplayerMessageType MessageParser::getMessageTypeForByteVector(const vector<unsigned char>& byteVector)
{
    if (byteVector.size() < sizeof(MultiplayerMessageHeader)) return MULTIPLAYER_MESSAGE_TYPE_UNKNOWN;
    
    MultiplayerMessageHeader* header = (MultiplayerMessageHeader*)&byteVector[0];
    
    // For now, this is the only messages we know
    if (header->messageType != MULTIPLAYER_MESSAGE_TYPE_OPPONENT_UPDATE &&
        header->messageType != MULTIPLAYER_MESSAGE_TYPE_OPPONENT_FIRE_BALL_UPDATE &&
        header->messageType != MULTIPLAYER_MESSAGE_TYPE_OPPONENT_AVATAR_NOTIFICATION)
    {
        return MULTIPLAYER_MESSAGE_TYPE_UNKNOWN;
    }
    
    // If this message comes from a newer client, then we can't really parse it
    if (header->protocolVersion > PROTOCOL_VERSION) {
        return MULTIPLAYER_MESSAGE_TYPE_UNKNOWN;
    }
    
    return (MultiplayerMessageType)header->messageType;
}

MultiplayerMessage* MessageParser::getMessageForTournamentP2PMessage(const nextpeer::TournamentP2PData &tournamentP2PData)
{
    MultiplayerMessageType type = getMessageTypeForByteVector(tournamentP2PData.message);
    
    if (type == MULTIPLAYER_MESSAGE_TYPE_UNKNOWN) return NULL; // unknown message
    
    switch (type) {
        case MULTIPLAYER_MESSAGE_TYPE_OPPONENT_UPDATE:
            return OpponentUpdate::createWithP2PData(tournamentP2PData);
            
        case MULTIPLAYER_MESSAGE_TYPE_OPPONENT_FIRE_BALL_UPDATE:
            return OpponentFireBallUpdateMessage::createWithP2PData(tournamentP2PData);
            
        case MULTIPLAYER_MESSAGE_TYPE_OPPONENT_AVATAR_NOTIFICATION:
            return OpponentAvatarNotificationMessage::createWithP2PData(tournamentP2PData);
            
        default:
            return NULL; // Unknown message
    }
}