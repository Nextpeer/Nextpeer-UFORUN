//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "OpponentUpdate.h"
#include "ViewPort.h"
#include "Box2D.h"
OpponentUpdate::OpponentUpdate()
{
}

MultiplayerMessageType OpponentUpdate::getMessageType() {
    return MULTIPLAYER_MESSAGE_TYPE_OPPONENT_UPDATE;
}

OpponentUpdate* OpponentUpdate::createWithP2PData(const TournamentP2PData& tournamentP2PData)
{
    OpponentUpdate* message = new OpponentUpdate();
    if (message->extractDataFromByteVector(tournamentP2PData.message)) {
        
        message->extractHeaderFromData(tournamentP2PData);
        message->autorelease();
        
        return message;
    }
    
    return NULL;
}

OpponentUpdate* OpponentUpdate::createWithHero(const Hero* hero)
{
    OpponentUpdate* message = new OpponentUpdate();
    
    // For some reason Cocos2D doesn't define getPosition() as const, so we'll const_cast here
    CCPoint screenPosition = const_cast<Hero*>(hero)->getPosition();
    
    b2Vec2 worldPos = ViewPort::getInstance()->screenToWorldCoordinate(screenPosition);
    
    message->_worldPositionX = worldPos.x;
    message->_worldPositionY = worldPos.y;
    message->_isHurt = hero->isHurt();
    message->_isStuck = hero->isStuck();
    
    b2Body *heroBody = hero->getB2Body();
    b2Vec2 currentLinearVel = heroBody->GetLinearVelocity();
    message->_linearVelocityX = currentLinearVel.x;
    message->_linearVelocityY = currentLinearVel.y;
    
    OpponentState opponentState = kOpponentUnknown;
    
    // Convert the Hero state to opponent state
    switch (hero->getHeroState()) {
        case kHeroReady:
            opponentState = kOpponentReady;
            break;
        case kHeroStopped:
            opponentState = kOpponentStopped;
            break;
        case kHeroPassedFinishLine:
            opponentState = kOpponentRunning;
            break;
        case kHeroRunning:
            // The hero can jump while runnning. For the hero that's actually the same state, while for the opponent it has seperate state
            if (hero->getIsJumping()) {
                opponentState = kOpponentJumping;
            }
            else {
                opponentState = kOpponentRunning;
            }
            break;
        default:
            break;
    }
    
    message->_state = opponentState;
    
    OpponentPowerUpState opponentPowerUpState = kOpponentPowerUpStateNone;
    
    // Convert the Hero power up state to opponent power up state
    switch (hero->getHeroPowerUpState()) {
        case kHeroPowerUpStateShield:
            opponentPowerUpState = kOpponentPowerUpStateShield;
            break;
            
        case kHeroPowerUpStateSpeedBoost:
            opponentPowerUpState = kOpponentPowerUpStateSpeedBoost;
            break;
        default:
            break;
    }
    
    message->mPowerUpState = opponentPowerUpState;
    
    message->autorelease();
    
    return message;
}

bool OpponentUpdate::extractDataFromByteVector(const vector<unsigned char>& byteVector)
{
    if (byteVector.size() < sizeof(OpponentUpdateMessageStruct)) {
        // This can't be a valid message
        return false;
    }
    
    OpponentUpdateMessageStruct* structPtr = (OpponentUpdateMessageStruct*)&byteVector[0];
    
    this->_worldPositionX = structPtr->worldPositionX;
    this->_worldPositionY = structPtr->worldPositionY;
    this->_linearVelocityX = structPtr->linearVelocityX;
    this->_linearVelocityY = structPtr->linearVelocityY;
    this->_state = (OpponentState)structPtr->state;
    this->mPowerUpState = (OpponentPowerUpState)structPtr->powerUpState;
    this->_isHurt = structPtr->isHurt;
    this->_isStuck = structPtr->isStuck;
    
    return true;
}

vector<unsigned char>& OpponentUpdate::toByteVector()
{
    OpponentUpdateMessageStruct messageStruct;
    
    messageStruct.header = this->getHeaderForDispatch();
    
    messageStruct.worldPositionX = this->_worldPositionX;
    messageStruct.worldPositionY = this->_worldPositionY;
    messageStruct.linearVelocityX = this->_linearVelocityX;
    messageStruct.linearVelocityY = this->_linearVelocityY;
    messageStruct.state = this->_state;
    messageStruct.isHurt = this->_isHurt;
    messageStruct.isStuck = this->_isStuck;
    messageStruct.powerUpState = this->mPowerUpState;
    static vector<unsigned char> byteVector = vector<unsigned char>(sizeof(messageStruct));
    memcpy(&byteVector[0], &messageStruct, sizeof(messageStruct));
    
    return byteVector;
}