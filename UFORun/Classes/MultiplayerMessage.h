//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__MultiplayerMessage__
#define __UFORun__MultiplayerMessage__

#include "cocos2d.h"
USING_NS_CC;

#include <vector>
#include <string>
using namespace std;

#include "MultiplayerProtocol.h"

#include "NextpeerNotifications.h"
using namespace nextpeer;


/**
 Abstract Class. Defines a multiplayer message in the game.
 */
class MultiplayerMessage : public CCObject
{
public:
    // Header
    CC_SYNTHESIZE(int, _protocolVersion, ProtocolVersion);
    CC_SYNTHESIZE(float, _timeStamp, TimeStamp);
    
    // Platform fields
    CC_SYNTHESIZE(string, _senderId, SenderId);
    CC_SYNTHESIZE(string, _senderName, SenderName);
    CC_SYNTHESIZE(bool, _isRecording, IsRecording);
    
    MultiplayerMessage();
    
    virtual vector<unsigned char>& toByteVector() = 0;
    virtual MultiplayerMessageType getMessageType() = 0; // Defines the message type
protected:
    virtual MultiplayerMessageHeader getHeaderForDispatch();
    virtual void extractHeaderFromData(const TournamentP2PData& tournamentP2PData); // Fill the superclass with data from the vector
};

#endif /* defined(__UFORun__MultiplayerMessage__) */
