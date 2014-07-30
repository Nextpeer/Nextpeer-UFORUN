//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "SimulatedOpponent.h"
#include "OpponentUpdate.h"
#include "CCNextpeer.h"
#include "ViewPort.h"
#include "MultiplayerGameState.h"

SimulatedOpponent::SimulatedOpponent(unsigned int countOfLivePlayers, GamePlayerProfile *profile, b2World* world, PlayerData *data) : Opponent(profile, world, data)
{
    _countOfLivePlayers = countOfLivePlayers;
    _lastKnownWorldPositionX = 0;
    _lastUpdateDistanceWorldPositionX = 0;
    _timeSinceHurtForward = 0;
    _gameClockOffset = 0;
    _hasTriedToRunBeforeTime = false;
}

SimulatedOpponent* SimulatedOpponent::create(unsigned int countOfLivePlayers, b2World* world, PlayerData *data)
{
    GamePlayerProfile *profile = GamePlayerProfileFactory::createProfileByTypeOrNull(data->getProfileType());
    
    SimulatedOpponent *simulatedOpponent = new SimulatedOpponent(countOfLivePlayers, profile, world, data);
    if (simulatedOpponent != NULL) {
        simulatedOpponent->autorelease();
        return simulatedOpponent;
    }
    CC_SAFE_DELETE(simulatedOpponent);
    return NULL;
}

void SimulatedOpponent::updateOpponentFromNetworkUpdate(OpponentUpdate* update) {
    if (!getIsHurt()) {
        Opponent::updateOpponentFromNetworkUpdate(update);
    }
}

void SimulatedOpponent::update (float dt) {
    Opponent::update(dt);
    
    // Increase the timer (this will keep track on the hurt effect avoid for the simulated opponent).
    _timeSinceHurtForward += dt;
    
    b2Vec2 worldPos = getB2Body()->GetPosition();
    if (worldPos.x > _lastKnownWorldPositionX) {
        _lastUpdateDistanceWorldPositionX = worldPos.x - _lastKnownWorldPositionX;
    }
    _lastKnownWorldPositionX = worldPos.x;
}

// Ignore hurt updates for this opponnet (can be hurt only by live players)
void SimulatedOpponent::applyHurtUpdate(OpponentUpdate* update) {
    if (_countOfLivePlayers == 0) return;
    if (_lastUpdateDistanceWorldPositionX == 0) return;
    
    // Ignore the update in case the simulated opponent is already hurt
    if (getIsHurt()) return;
    
    // If the update marks hurt, move forward (skip).
    // Keep track on the time since we started the hurt effect so extra messages will not make the opponent simulate hurt one more time.
    if (update->getIsHurt() && (_timeSinceHurtForward > HURT_BY_FIRE_BALL_TIMEOUT)) {
        // Fast forward will take place on all connected devices. Make sure to tell the server to fast forward relative to the amount of live players (still connected).
        CCNextpeer::getInstance()->requestFastForwardRecording(getPlayerData()->getPlayerId().c_str(), HURT_BY_FIRE_BALL_TIMEOUT/_countOfLivePlayers);
        
        float scoreUpdate = _lastUpdateDistanceWorldPositionX*HURT_BY_FIRE_BALL_TIMEOUT/_countOfLivePlayers;
        CCNextpeer::getInstance()->reportScoreModifierForRecording(getPlayerData()->getPlayerId().c_str(), scoreUpdate);
        
        // Make sure to adjuts the clock with the time we just fast forwarded (otherwise the messages will be out of sync)
        _gameClockOffset += HURT_BY_FIRE_BALL_TIMEOUT;
        
        _timeSinceHurtForward = 0;
    }
}

void SimulatedOpponent::simulateHitByFireBallIfRequired(){
    if (_countOfLivePlayers == 0) return;
    if (_lastUpdateDistanceWorldPositionX == 0) return;
    if (getIsHurt()) return;
    
    // The score which this opponent was supposed to get in the next HURT_BY_FIRE_BALL_TIMEOUT seconds.
    // Including also the bonus time which will be decreased from this opponent.
    // Dividing for the count of live players as all live players will calculate this for the same simulated opponent.
    float scoreDelta = _lastUpdateDistanceWorldPositionX*PLAYER_SCORE_DISTANCE_MODIFIER*HURT_BY_FIRE_BALL_TIMEOUT/_countOfLivePlayers + HURT_BY_FIRE_BALL_TIMEOUT* PLAYER_BONUS_PER_SECOND/_countOfLivePlayers;
    
    setIsHurt(true);
    
    // Make sure the simulated opponent stop at the spot
    b2Body* playerBody = getB2Body();
    playerBody->SetLinearVelocity(b2Vec2(0,0));
    setSpeedX(0);
    
    // Pause the recording for the hurt time!
    CCNextpeer::getInstance()->requestPauseRecording(getPlayerData()->getPlayerId().c_str());
    this->scheduleOnce(schedule_selector(SimulatedOpponent::hurtAnimationFinished), HURT_BY_FIRE_BALL_TIMEOUT);
    
    CCNextpeer::getInstance()->reportScoreModifierForRecording(getPlayerData()->getPlayerId().c_str(), -scoreDelta);
    
    showPlayerEffectAnimation(kPlayerEffectAnimationHurt);
}

void SimulatedOpponent::hurtAnimationFinished(){
    stopPlayerEffectAnimation();
    
    // Resume the recording (hurt timeout finished)
    CCNextpeer::getInstance()->requestResumeRecording(getPlayerData()->getPlayerId().c_str());
    
    setSpeedX(PLAYER_DEFAULT_LINEAR_SPEED_X);
    
    // We need to adjust the game clock with the offset of the time we have spent on the fireball (otherwise the messages will be out of sync).
    _gameClockOffset -= (HURT_BY_FIRE_BALL_TIMEOUT);
    
    // This will make sure the opponent will switch back to run animation
    showRunAnimation();
    setIsHurt(false);
}