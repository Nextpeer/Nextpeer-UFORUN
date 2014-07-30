//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "MultiplayerMessage.h"
#include "GameClock.h"

MultiplayerMessage::MultiplayerMessage()
{
    _protocolVersion = PROTOCOL_VERSION;
    _timeStamp = 0;
}

MultiplayerMessageHeader MultiplayerMessage::getHeaderForDispatch()
{
    MultiplayerMessageHeader header;
    header.protocolVersion = PROTOCOL_VERSION;
    header.timeStamp = GameClock::getInstance()->getClock();
    header.messageType = getMessageType();
    return header;
}

void MultiplayerMessage::extractHeaderFromData(const TournamentP2PData& tournamentP2PData)
{
    const vector<unsigned char>& byteVector = tournamentP2PData.message;
    MultiplayerMessageHeader* header = (MultiplayerMessageHeader*)&byteVector[0];
    
    // Fill fields from the header
    this->setTimeStamp(header->timeStamp);
    this->setProtocolVersion(header->protocolVersion);
    
    // Fill fields from the platform
    this->setSenderId(tournamentP2PData.playerId);
    this->setSenderName(tournamentP2PData.playerName);
    this->setIsRecording(tournamentP2PData.playerIsRecording);
}