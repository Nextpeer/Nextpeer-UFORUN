//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__SimulatedOpponent__
#define __UFORun__SimulatedOpponent__

#include "Opponent.h"
#include "PlayerSpriteProfile.h"
#include "PlayerData.h"

class OpponentUpdate;

/**
 Simulate a recordings of an opponent. As the opponent will run on the same level as the current hero we will need to parse the interaction between the other "live" players to the recording. For example getting shot or avoiding it.
 */
class SimulatedOpponent : public Opponent
{
private:
    bool _hasTriedToRunBeforeTime;
    float _lastKnownWorldPositionX;
    float _lastUpdateDistanceWorldPositionX;
    float _timeSinceHurtForward;
    float _gameClockOffset;
    unsigned int _countOfLivePlayers;
    
    void hurtAnimationFinished();
protected:
    SimulatedOpponent(unsigned int countOfLivePlayers, GamePlayerProfile *profile, b2World* world, PlayerData *data);
    
    virtual void updateOpponentFromNetworkUpdate(OpponentUpdate* update);
    virtual void applyHurtUpdate(OpponentUpdate* update);
    
public:
    static SimulatedOpponent* create(unsigned int countOfLivePlayers, b2World* world, PlayerData *data);
    
    virtual void update (float dt);
    virtual void simulateHitByFireBallIfRequired();
    
    // The game clock offset, in case the recording got stopped/rewind
    virtual float getClockOffset() const {
        return _gameClockOffset;
    }
};


#endif /* defined(__UFORun__SimulatedOpponent__) */
