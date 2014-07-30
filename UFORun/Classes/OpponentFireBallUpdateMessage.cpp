//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "OpponentFireBallUpdateMessage.h"
#include "ViewPort.h"

OpponentFireBallUpdateMessage::OpponentFireBallUpdateMessage()
{
}

MultiplayerMessageType OpponentFireBallUpdateMessage::getMessageType() {
    return MULTIPLAYER_MESSAGE_TYPE_OPPONENT_FIRE_BALL_UPDATE;
}

OpponentFireBallUpdateMessage* OpponentFireBallUpdateMessage::createWithP2PData(const nextpeer::TournamentP2PData &tournamentP2PData)
{
    OpponentFireBallUpdateMessage* message = new OpponentFireBallUpdateMessage();
    if (message->extractDataFromByteVector(tournamentP2PData.message)) {
        
        message->extractHeaderFromData(tournamentP2PData);
        message->autorelease();
        
        return message;
    }
    
    return NULL;
}

OpponentFireBallUpdateMessage* OpponentFireBallUpdateMessage::createWithFireBall(const HeroFireBall* fireBall)
{
    OpponentFireBallUpdateMessage* message = new OpponentFireBallUpdateMessage();
    message->autorelease();
    
    // For some reason Cocos2D doesn't define getPosition() as const, so we'll const_cast here
    CCPoint screenPosition = const_cast<HeroFireBall*>(fireBall)->getPosition();
    
    b2Vec2 worldPos = ViewPort::getInstance()->screenToWorldCoordinate(screenPosition);
    
    message->_worldPositionX = worldPos.x;
    message->_worldPositionY = worldPos.y;
    message->_originPowerUpId = fireBall->getOriginPowerUpId();
    
    return message;
}

vector<unsigned char>& OpponentFireBallUpdateMessage::toByteVector()
{
    OpponentFireBallUpdateMessageStruct messageStruct;
    
    messageStruct.header = this->getHeaderForDispatch();
    
    messageStruct.worldPositionX = this->_worldPositionX;
    messageStruct.worldPositionY = this->_worldPositionY;
    messageStruct.originPowerUpId = this->_originPowerUpId;
    
    static vector<unsigned char> byteVector = vector<unsigned char>(sizeof(messageStruct));
    memcpy(&byteVector[0], &messageStruct, sizeof(messageStruct));
    
    return byteVector;
}

bool OpponentFireBallUpdateMessage::extractDataFromByteVector(const vector<unsigned char>& byteVector)
{
    if (byteVector.size() < sizeof(OpponentFireBallUpdateMessageStruct)) {
        // This can't be a valid message
        return false;
    }
    
    OpponentFireBallUpdateMessageStruct* structPtr = (OpponentFireBallUpdateMessageStruct*)&byteVector[0];
    
    this->_worldPositionX = structPtr->worldPositionX;
    this->_worldPositionY = structPtr->worldPositionY;
    this->_originPowerUpId = structPtr->originPowerUpId;
    
    return true;
}