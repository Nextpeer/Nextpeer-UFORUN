//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "NextpeerDelegate.h"

#include "NextpeerNotifications.h"
#include "GameLayer.h"
#include "MainMenuLayer.h"
#include "Rand.h"
#include "MultiplayerGameState.h"
#include "NextpeerPlayer.h"
#include "PlayerData.h"

using namespace nextpeer;

NextpeerDelegate::NextpeerDelegate()
{
    _currentGameState = MultiplayerGameState::create();
    _currentGameState->retain();
}

NextpeerDelegate::~NextpeerDelegate()
{
    CC_SAFE_RELEASE_NULL(_currentGameState);
    
}

void NextpeerDelegate::registerForEvents()
{
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(NextpeerDelegate::nextpeerDidStartTournament),
                                                                  NEXTPEER_NOTIFICATION_TOURNAMENT_STARTED,
                                                                  NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(NextpeerDelegate::nextpeerDidEndTournament),
                                                                  NEXTPEER_NOTIFICATION_TOURNAMENT_ENDED,
                                                                  NULL);
}

void NextpeerDelegate::unhookEvents()
{
    CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
}

void NextpeerDelegate::nextpeerDidStartTournament(CCObject *startData)
{
    _currentGameState->reset();
    
    // Add opponents
    TournamentStartData* tournamentStartData = (TournamentStartData*)startData;
    CCObject* it = NULL;
    CCARRAY_FOREACH(tournamentStartData->players, it)
    {
        NextpeerPlayer *nextpeerPlayer = static_cast<NextpeerPlayer*>(it);
        
        // Cast the NextpeerPlayer class to PlayerData class which will contain the relevant data about this player
        _currentGameState->addOpponent(PlayerData::create(nextpeerPlayer->getPlayerId(), nextpeerPlayer->getPlayerName(), nextpeerPlayer->getIsRecording()));
    }
    
    // Seed the random number generator with the seed that the server gave us
    Rand::seed(tournamentStartData->tournamentRandomSeed);
    
    // Tournament start, switch to the game scene
    CCScene *pScene = GameLayer::scene(_currentGameState);
    CCDirector::sharedDirector()->replaceScene(pScene);
}

void NextpeerDelegate::nextpeerDidEndTournament()
{
    // Tournament end, switch back to the main menu
    CCDirector::sharedDirector()->replaceScene(MainMenuLayer::scene());
}