//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef UFORun_MultiplayerProtocol_h
#define UFORun_MultiplayerProtocol_h

#define PROTOCOL_VERSION 1

typedef struct __attribute__ ((__packed__)) {
    uint32_t protocolVersion;
    uint32_t messageType;
    float timeStamp;
    
} MultiplayerMessageHeader;

typedef enum {
    MULTIPLAYER_MESSAGE_TYPE_UNKNOWN = 0,
    MULTIPLAYER_MESSAGE_TYPE_OPPONENT_UPDATE = 1, // Updates on the opponent state
    MULTIPLAYER_MESSAGE_TYPE_OPPONENT_FIRE_BALL_UPDATE = 2, // Updates on the opponent's fireball (laser) state
    MULTIPLAYER_MESSAGE_TYPE_OPPONENT_AVATAR_NOTIFICATION = 3 // Information about the opponent avatar
} MultiplayerMessageType;

#endif
