//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__MultiplayerGameState__
#define __UFORun__MultiplayerGameState__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Box2D.h"
#include "PlayerSpriteProfile.h"

class Hero;
class Opponent;
class HeroFireBall;
class OpponentFireBall;
class PlayerData;
class OpponentUpdate;
class OpponentFireBallUpdateMessage;
class OpponentAvatarNotificationMessage;

// Exposed notification for the multiplayer state
#define MULTIPLAYER_NOTIFICATION_OPPONENTS_SPRITES_READY "multiplayer_opponents_sprites_ready"
#define MULTIPLAYER_NOTIFICATION_SHOULD_START_GAME "multiplayer_should_start_game"

/**
 Manage the different multiplayer states (dispatch & receive), holds the opponent details and so on.
 */
class MultiplayerGameState : public CCObject
{
private:
    
    MultiplayerGameState();
    ~MultiplayerGameState();
    
    CCDictionary* _opponentsData; // The Opponent raw data (as we get it from Nextpeer) Of type Key:String (Player Id) Value:PlayerData
    CCDictionary* _opponentsSprite; //  Of type Key:String (Player Id) Value:Opponent -> holds the Opponent sprites for the game scene
    CCDictionary* _opponentFireBalls; // Of type Key:String (origin power up id + opponent id) Value:OpponentFireBall -> holds the opponent fire balls as rerported from the network
    CCDictionary *_stoppedFireBalls; // Of type Key:String (origin power up id + opponent id) Value:OpponentFireBall -> holds the records of opponents which already stopped so there will not be any network miss (when the client marked as stopped but network stats as moving).
    CCArray* _opponentsSpriteQueue; // In case the avatar sprites message arrived before the current client managed to dispatch its call
    unsigned int _countOfLivePlayers; // Count of live players, hence not recordings (including the current player)
    bool _hasNotifiedOpponentsSpriteReady; // Making sure the game state notified only once on this state
    
    void applyOpponentUpdate(OpponentUpdate* update); // Make sure the given opponent exists and apply the network update on the opponent sprite
    void applyOpponentFireBallUpdate(OpponentFireBallUpdateMessage* fireBallUpdate); // Make sure the given opponent exists (valid fireball) and then apply the network update on it.
    void applyOpponentAvatarUpdate(OpponentAvatarNotificationMessage* avatarNotificationMessage); // Apply the avatar update on the opponent
    
    void addOpponentSprite(PlayerData* playerData); // Creating an Opponent sprite based on the PlayerData
    void notifyMultiplayerGameStateReady(); // Raise the MULTIPLAYER_NOTIFICATION_OPPONENTS_SPRITES_READY notification (if didn't already), which alert that opponent sprites loaded.
    bool isMultiplayerGameStateReady(); // Did all of the opponents sent their avatar selection
    // CCNotificationCenter
    void incomingNextpeerDataPacket(CCObject* packet); // Accept an incoming player to player Nextpeer message (of type TournamentP2PData)
    const string createFireBallKey(string opponentId, int powerUpId); // Helper method. Create a unique key for a fireball based on the opponent id and the power up id which used to shot it.
    Opponent *createOpponentByData(b2World* world, PlayerData *data); // Create an OpponentSprite in the Box2D world with the given PlayerData
public:
    static MultiplayerGameState* create(); // Create the instance
    
    void reset(); // Reset the state of the multiplayer game
    void listenForIncomingPackets(); // Register for Nextpeer notification about player to player messages
    void addOpponent(PlayerData *opponent); // Add an Opponent to the state
    CCArray* getAllOpponents(); // Copy of the array (type Opponent)
    CCArray* getAllOpponentFireBalls(); // Copy of the array (type OpponentFireBall)
    
    /* Dispatch Messages */
    void dispatchUpdateForHero(Hero* hero); // Dispatch an update about the Hero (will create OpponentUpdate message and dispatch via Nextpeer's unreliable channel
    void dispatchUpdateForFireBall(HeroFireBall* fireBall); // Dispatch an update about the Hero fire ball (will create OpponentFireBallUpdateMessage message and dispatch via Nextpeer's unreliable channel
    void dispatchAvatarNotification(const PlayerData& playerData); // Dispatch an "Avatar Details" message about the Hero fire ball (will create OpponentAvatarNotificationMessage message and dispatch via Nextpeer's reliable channel
    
    void flushOpponentsWaitingToLoadQueue(); // In case there are any opponents who waiting to load due to loading thread race (in case some opponents arrived before the level load method called)
    void waitForOpponentsSpritesTimedOut(); // The timeout for the avatar messages has expired (in case there are invalid recordings which doesn't have such message this timeout method will trigger).
    void trimOpponentFireBalls(); // Clear the fire balls which stopped.
    
    CC_SYNTHESIZE(b2World*, _world, World); // Weak Ref for the Box2D world (used to place an opponent or a fireball in the level)
    CC_SYNTHESIZE(bool, _canLoadOpponents, CanLoadOpponents); // When the Box2D world is ready to accept new items (in case the scene has not loaeded yet we should add the details to the queue)
};

#endif /* defined(__UFORun__MultiplayerGameState__) */