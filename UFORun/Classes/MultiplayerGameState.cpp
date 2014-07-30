//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "MultiplayerGameState.h"
#include "MessageParser.h"

#include "Hero.h"
#include "PlayerData.h"
#include "PowerUp.h"
#include "HeroFireBall.h"
#include "OpponentFireBall.h"
#include "OpponentUpdate.h"
#include "OpponentFireBallUpdateMessage.h"
#include "OpponentAvatarNotificationMessage.h"

#include "Opponent.h"
#include "OpponentFireBall.h"
#include "CCNextpeer.h"
#include "ViewPort.h"
#include "Rand.h"

using namespace nextpeer;

#include <cmath>

MultiplayerGameState* MultiplayerGameState::create()
{
    MultiplayerGameState* state = new MultiplayerGameState();
    state->autorelease();
    
    return state;
}

MultiplayerGameState::MultiplayerGameState()
{
    _opponentsSprite = CCDictionary::create();
    _opponentsSprite->retain();
    _opponentsData = CCDictionary::create();
    _opponentsData->retain();
    _opponentFireBalls = CCDictionary::create();
    _opponentFireBalls->retain();
    _stoppedFireBalls = CCDictionary::create();
    _stoppedFireBalls->retain();
    _opponentsSpriteQueue = CCArray::create();
    _opponentsSpriteQueue->retain();
    _canLoadOpponents = false;
    _hasNotifiedOpponentsSpriteReady = false;
    
    _world = NULL;
}

MultiplayerGameState::~MultiplayerGameState()
{
    this->reset();
    
    CC_SAFE_RELEASE_NULL(_opponentsSprite);
    CC_SAFE_RELEASE_NULL(_opponentsData);
    CC_SAFE_RELEASE_NULL(_opponentFireBalls);
    CC_SAFE_RELEASE_NULL(_stoppedFireBalls);
    CC_SAFE_RELEASE_NULL(_opponentsSpriteQueue);
}

void MultiplayerGameState::reset()
{
    if (_opponentsSprite->count() > 0) {
        _opponentsSprite->removeAllObjects();
    }
    
    if (_opponentsData->count() > 0) {
        _opponentsData->removeAllObjects();
    }
    
    if (_opponentFireBalls->count() > 0) {
        _opponentFireBalls->removeAllObjects();
    }
    
    if (_stoppedFireBalls->count() > 0) {
        _stoppedFireBalls->removeAllObjects();
    }
    
    if (_opponentsSpriteQueue->count() > 0) {
        _opponentsSpriteQueue->removeAllObjects();
    }
    
    _canLoadOpponents = false;
    _hasNotifiedOpponentsSpriteReady = false;
    
    CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
}

void MultiplayerGameState::listenForIncomingPackets()
{
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(MultiplayerGameState::incomingNextpeerDataPacket),
                                                                  NEXTPEER_NOTIFICATION_INCOMING_DATA_PACKET,
                                                                  NULL);
}

void MultiplayerGameState::incomingNextpeerDataPacket(CCObject *packet)
{
    if (!CCNextpeer::getInstance()->isCurrentlyInTournament()) return;
    
    TournamentP2PData* p2pData = (TournamentP2PData*)packet;
    
    MultiplayerMessage* message = MessageParser::getMessageForTournamentP2PMessage(*p2pData);
    
    // Must be an unknown message type
    if (!message) return;
    
    MultiplayerMessageType type = message->getMessageType();
    
    switch (type) {
        case MULTIPLAYER_MESSAGE_TYPE_OPPONENT_UPDATE: {
            
            OpponentUpdate* update = (OpponentUpdate*)message;
            this->applyOpponentUpdate(update);
            break;
        }
            
        case MULTIPLAYER_MESSAGE_TYPE_OPPONENT_FIRE_BALL_UPDATE: {
            
            OpponentFireBallUpdateMessage* fireBallUpdate = (OpponentFireBallUpdateMessage*)message;
            this->applyOpponentFireBallUpdate(fireBallUpdate);
            break;
        }
            
        case MULTIPLAYER_MESSAGE_TYPE_OPPONENT_AVATAR_NOTIFICATION: {
            
            OpponentAvatarNotificationMessage* avatarNotification = (OpponentAvatarNotificationMessage*)message;
            this->applyOpponentAvatarUpdate(avatarNotification);
            break;
        }
            
        default:{
            break;
        }
    }
}

void MultiplayerGameState::addOpponent(PlayerData *opponent) {
    _opponentsData->setObject(opponent, opponent->getPlayerId());
}

CCArray* MultiplayerGameState::getAllOpponents()
{
    if (_opponentsSprite == NULL || _opponentsSprite->count() <= 0) return NULL;
    CCArray* opponents = CCArray::createWithCapacity(_opponentsSprite->count());
    
    CCDictElement* dictElement = NULL;
    CCDICT_FOREACH(_opponentsSprite, dictElement)
    {
        opponents->addObject((Opponent*)dictElement->getObject());
    }
    
    return opponents;
}

CCArray* MultiplayerGameState::getAllOpponentFireBalls() {
    if (_opponentFireBalls == NULL || _opponentFireBalls->count() <= 0) return NULL;
    CCArray* opponentFireBalls = CCArray::createWithCapacity(_opponentFireBalls->count());
    
    CCDictElement* dictElement = NULL;
    CCDICT_FOREACH(_opponentFireBalls, dictElement)
    {
        OpponentFireBall *opponentFireBall = (OpponentFireBall*)dictElement->getObject();
        opponentFireBalls->addObject(opponentFireBall);
    }
    
    return opponentFireBalls;
}

void MultiplayerGameState::dispatchUpdateForHero(Hero* hero)
{
    // Generate an opponent update and then send it out
    OpponentUpdate* msg = OpponentUpdate::createWithHero(hero);
    vector<unsigned char>& byteVector = msg->toByteVector();
    
    CCNextpeer::getInstance()->unreliablePushDataToOtherPlayers(&byteVector[0], byteVector.size());
}

void MultiplayerGameState::dispatchUpdateForFireBall(HeroFireBall* fireBall) {
    
    // Dispatch messages only on moving fireballs
    if (fireBall->getFireBallState() != kFireBallMoving) return;
    
    OpponentFireBallUpdateMessage *msg = OpponentFireBallUpdateMessage::createWithFireBall(fireBall);
    vector<unsigned char>& byteVector = msg->toByteVector();
    
    CCNextpeer::getInstance()->unreliablePushDataToOtherPlayers(&byteVector[0], byteVector.size());
}

void MultiplayerGameState::dispatchAvatarNotification(const PlayerData& playerData)
{
    OpponentAvatarNotificationMessage* message = OpponentAvatarNotificationMessage::createWithPlayerData(playerData);
    vector<unsigned char>& buf = message->toByteVector();
    
    CCNextpeer::getInstance()->pushDataToOtherPlayers(&buf[0], buf.size());
}

void MultiplayerGameState::applyOpponentUpdate(OpponentUpdate *update)
{
    string id = update->getSenderId();
    Opponent* opponent = static_cast<Opponent*>(_opponentsSprite->objectForKey(id));
    
    if (!opponent) {
        // This is weird, we don't know this opponent. This update will be ignored
        return;
    }
    
    opponent->applyOpponentUpdate(update);
}

const string MultiplayerGameState::createFireBallKey(string opponentId, int powerUpId) {
    
    char fireBallKey[100] = {0};
    sprintf(fireBallKey, "%s_%i", opponentId.c_str(), powerUpId);
    return fireBallKey;
}

Opponent *MultiplayerGameState::createOpponentByData(b2World* world, PlayerData *data) {
    return Opponent::create(world, data);
}

void MultiplayerGameState::applyOpponentFireBallUpdate(OpponentFireBallUpdateMessage* fireBallUpdate) {
    
    // Safe guard, from creating an object without Box2D world
    if (_world == NULL) {
        CCLog("Something went wrong - could not apply the opponent fire ball message as the Box2D world is missing");
        return;
    }
    
    string opponentId = fireBallUpdate->getSenderId();
    
    Opponent* opponent = static_cast<Opponent*>(_opponentsSprite->objectForKey(opponentId));
    if (!opponent) return; // this shouldn't happen (don't have the opponent sprite)
    
    uint32_t powerUpId = fireBallUpdate->getOriginPowerUpId();
    
    // We know that each power up can be used by one opponent at max, so we use that combination as a key for the fire balls dictionary
    string fireBallKey = createFireBallKey(opponentId, powerUpId);
    
    // The object exists in the stopped fire ball dictionary, that means we should not accept any network updates for this fire ball.
    if (_stoppedFireBalls->objectForKey(fireBallKey)) {
        return;
    }
    
    CCObject *pOpponentFireBall = _opponentFireBalls->objectForKey(fireBallKey);
    
    OpponentFireBall *fireBall = NULL;
    
    // New fire ball, add it to the dictionary
    if (pOpponentFireBall == NULL) {
        fireBall = OpponentFireBall::create(powerUpId, opponent, _world);
        
        // Set the new fire ball to the opponent fire balls dictionary
        _opponentFireBalls->setObject(fireBall, fireBallKey);
    }
    // Update the fire ball with the new update
    else {
        fireBall = static_cast<OpponentFireBall*>(pOpponentFireBall);
        
        // This fireball has stopped already, ignore this update
        if (fireBall->getFireBallState() == kFireBallStopped) {
            fireBall = NULL;
        }
    }
    
    // Apply the update on the fire ball if one exist
    if (fireBall != NULL) {
        fireBall->applyOpponentFireBallUpdate(fireBallUpdate);
    }
}

void MultiplayerGameState::applyOpponentAvatarUpdate(OpponentAvatarNotificationMessage* avatarNotificationMessage)
{
    PlayerData* playerData = avatarNotificationMessage->toPlayerData();
    addOpponentSprite(playerData);
}

void MultiplayerGameState::addOpponentSprite(PlayerData* playerData)
{
    if (_canLoadOpponents) { // We can load this opponent right now
        // Create the opponent, place it under zero point -> it will be arranged later on.
        Opponent *opponent = createOpponentByData(_world, playerData);
        _opponentsSprite->setObject(opponent, playerData->getPlayerId());
        
        if (isMultiplayerGameStateReady()) {
            notifyMultiplayerGameStateReady();
        }
    }
    else {
        // Add to the queue to be added later on
        _opponentsSpriteQueue->addObject(playerData);
    }
    
    PlayerData* data = static_cast<PlayerData*>(_opponentsData->objectForKey(playerData->getPlayerId()));
    data->setProfileType(playerData->getProfileType());
}

void MultiplayerGameState::notifyMultiplayerGameStateReady()
{
    // Don't notify more than once
    if (_hasNotifiedOpponentsSpriteReady) return;
    
    _hasNotifiedOpponentsSpriteReady = true;
    
    CCNotificationCenter::sharedNotificationCenter()->postNotification(MULTIPLAYER_NOTIFICATION_OPPONENTS_SPRITES_READY);
}

void MultiplayerGameState::flushOpponentsWaitingToLoadQueue()
{
    CCObject* it = NULL;
    CCARRAY_FOREACH(_opponentsSpriteQueue, it)
    {
        PlayerData* data = static_cast<PlayerData*>(it);
        Opponent* opponent = createOpponentByData(_world, data);
        _opponentsSprite->setObject(opponent, data->getPlayerId());
    }
    
    // Flush
    _opponentsSpriteQueue->removeAllObjects();
    
    if (isMultiplayerGameStateReady()) {
        notifyMultiplayerGameStateReady();
    }
}

void MultiplayerGameState::waitForOpponentsSpritesTimedOut()
{
    if (_hasNotifiedOpponentsSpriteReady) {
        return;
    }
    
    bool canStart = true;
    CCDictElement* it = NULL;
    CCDICT_FOREACH(_opponentsData, it)
    {
        PlayerData* player = static_cast<PlayerData*>(it->getObject());
        if (player->getIsRecording()) {
            if (!_opponentsSprite->objectForKey(player->getPlayerId())) {
                // Fill a random sprite for this recording
                GamePlayerProfileType type = (GamePlayerProfileType)Rand::generate(0, GamePlayerProfileTypeMax);
                player->setProfileType(type);
                addOpponentSprite(player);
            }
        }
        else {
            if (_opponentsSprite->objectForKey(player->getPlayerId())) {
                canStart = false;
            }
        }
    }
    
    if (canStart) {
        notifyMultiplayerGameStateReady();
    }
}

void MultiplayerGameState::trimOpponentFireBalls() {
    
    // Move from the active fire balls to the stopped
    CCArray *activeOpponentFireBallsKeys = _opponentFireBalls->allKeys();
    if(activeOpponentFireBallsKeys)
    {
        for(int i = 0; i < (int)activeOpponentFireBallsKeys->count(); ++i){
            std::string opponentFireBallKey = std::string(((CCString*)activeOpponentFireBallsKeys->objectAtIndex(i))->getCString());
            CCObject *pOpponentFireBall = _opponentFireBalls->objectForKey(opponentFireBallKey);
            OpponentFireBall *fireBall = static_cast<OpponentFireBall*>(pOpponentFireBall);
            
            // The fireball stopped either by timeout, hit an obstacle or opponent. It is no longer valid. Move it to the stopped fire balls (so we will not get any more updates on it).
            if (fireBall->getFireBallState() == kFireBallStopped) {
                _stoppedFireBalls->setObject(fireBall, opponentFireBallKey);
                _opponentFireBalls->removeObjectForKey(opponentFireBallKey);
            }
        }
    }
    
    // Clear the stopped fire balls (there is no reason to get any network updates on them)
    CCArray *stoppedOpponentFireBallsKeys = _stoppedFireBalls->allKeys();
    if(stoppedOpponentFireBallsKeys)
    {
        for(int i = 0; i < (int)stoppedOpponentFireBallsKeys->count(); ++i){
            std::string opponentFireBallKey = std::string(((CCString*)stoppedOpponentFireBallsKeys->objectAtIndex(i))->getCString());
            CCObject *pOpponentFireBall = _stoppedFireBalls->objectForKey(opponentFireBallKey);
            OpponentFireBall *fireBall = static_cast<OpponentFireBall*>(pOpponentFireBall);
            
            // The fireball stopped, the time since the last update passed the flying time of the fireball, we can remove it now of out the memory.
            if (fireBall->canBeRemoved()) {
                _stoppedFireBalls->removeObjectForKey(opponentFireBallKey);
            }
        }
    }
}

bool MultiplayerGameState::isMultiplayerGameStateReady()
{
    return _opponentsSprite->count() == _opponentsData->count();
}