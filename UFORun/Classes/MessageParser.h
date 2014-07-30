//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__MessageParser__
#define __UFORun__MessageParser__

#include "MultiplayerMessage.h"
#include "MultiplayerProtocol.h"
#include "NextpeerNotifications.h"
using namespace nextpeer;

/**
 Factory for MultiplayerMessage by TournamentP2PData.
 */
class MessageParser
{
public:
    static MultiplayerMessage* getMessageForTournamentP2PMessage(const TournamentP2PData& tournamentP2PData);
protected:
    static MultiplayerMessageType getMessageTypeForByteVector(const vector<unsigned char>& byteVector);
};


#endif /* defined(__UFORun__MessageParser__) */
