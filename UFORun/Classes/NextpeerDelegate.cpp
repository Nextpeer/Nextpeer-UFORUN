#include "NextpeerDelegate.h"

#include "NextpeerNotifications.h"
#include "GameLayer.h"
#include "MainMenuLayer.h"
#include "Rand.h"

using namespace nextpeer;

NextpeerDelegate::NextpeerDelegate()
{
}

NextpeerDelegate::~NextpeerDelegate()
{
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
    TournamentStartData* tournamentStartData = (TournamentStartData*)startData;
    
    // Seed the random number generator with the seed that the server gave us
    Rand::seed(tournamentStartData->tournamentRandomSeed);
    
    // Tournament end, switch to the game scene
    CCDirector::sharedDirector()->replaceScene(GameLayer::scene());
}

void NextpeerDelegate::nextpeerDidEndTournament()
{
    // Tournament end, switch back to the main menu
    CCDirector::sharedDirector()->replaceScene(MainMenuLayer::scene());
}