//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "PlayerData.h"

PlayerData* PlayerData::create(string playerId, string playerName, bool isRecording) {
    PlayerData *playerData = new PlayerData(playerId, playerName, isRecording);
    playerData->autorelease();
    return playerData;
}

PlayerData::PlayerData(string playerId, string playerName, bool isRecording) {
    _playerId = playerId;
    _playerName = playerName;
    _isRecording = isRecording;
}