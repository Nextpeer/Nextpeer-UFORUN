//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__PlayerData__
#define __UFORun__PlayerData__

#include "cocos2d.h"
#include "PlayerSpriteProfile.h"

USING_NS_CC;
using namespace std;

/**
 Container for player relevent data.
 */
class PlayerData : public CCObject {
private:
    string _playerId;
    string _playerName;
    bool _isRecording;
    
    PlayerData(string playerId, string playerName, bool isRecording);
public:
    static PlayerData* create(string playerId, string playerName, bool isRecording);
    
    string getPlayerId() const{
        return _playerId;
    }
    
    string getPlayerName() const {
        return _playerName;
    }
    
    bool getIsRecording() const {
        return _isRecording;
    }
    
    CC_SYNTHESIZE(GamePlayerProfileType, _profileType, ProfileType);
};

#endif /* defined(__UFORun__PlayerData__) */
