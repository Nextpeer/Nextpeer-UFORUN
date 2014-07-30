//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "GameLayer.h"

#include "CCNextpeer.h"
#include "Hero.h"
#include "ViewPort.h"
#include "GameContactListener.h"
#include "PowerUp.h"
#include "CCSortableArray.h"
#include "MainMenuLayer.h"
#include "HeroFireBall.h"
#include "OpponentFireBall.h"
#include "GameClock.h"
#include "MultiplayerGameState.h"
#include "Opponent.h"
#include "GameLevel.h"
#include "Rand.h"

#define HUD_ITEMS_SPACING 10.0f

#define MAX_WAIT_FOR_OPPONENT_SPRITES_SIGNAL_SECONDS 5
#define PRE_GAME_COUNT_DOWN_SECONDS 4

#define MAX_WAIT_FOR_GAMES_IMAGES 4 // Should be the same as PRE_GAME_COUNT_DOWN_SECONDS (from the InGame.plist atlas)

#define kStartGameSyncEventName "com.nextpeer.uforun.syncevet.startgame"
#define kStartGameSyncEventTimeout 10.0

// The maximum time in the level that will yield a bonus
#define LEVEL_MAX_TIME 100.0f

#define BM_FONT_NAME "font.fnt"
#define TTF_FONT_NAME "Thonburi"
#define TTF_BONUS_TITLE_FONT_SIZE 30
#define TTF_LOADING_TITLE_FONT_SIZE 40

using namespace nextpeer;

typedef enum
{
    kGameLayerZOrderDefault = 10,
    kGameLayerZOrderHUD = 20
    
} GameLayerZOrder;

CCScene* GameLayer::scene(MultiplayerGameState* multiplayerGameState)
{
	CCScene * scene = NULL;
	do
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
        
        // 'layer' is an autorelease object
        GameLayer *layer = GameLayer::create(multiplayerGameState);
		CC_BREAK_IF(! layer);
        
		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);
    
	// return the scene
	return scene;
}

GameLayer::~GameLayer()
{
    CC_SAFE_DELETE(_loader);
    CC_SAFE_DELETE(_world);
    
    CC_SAFE_RELEASE_NULL(_multiplayerGameState);
    
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_gameLayer);
    CC_SAFE_RELEASE_NULL(_loadingLayer);
    CC_SAFE_RELEASE_NULL(_hudLayer);
    CC_SAFE_RELEASE_NULL(_collectedPowerUp);
    CC_SAFE_RELEASE_NULL(_heroFireBalls);
    CC_SAFE_RELEASE_NULL(_powerUpBoxes);
    
    // Remove all observers
    CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
}

// on "init" you need to initialize your instance
bool GameLayer::init(MultiplayerGameState* multiplayerGameState)
{
	bool bRet = false;
	do
	{
        //////////////////////////////
        // 1. super init first
        CC_BREAK_IF(! CCLayer::init());
        
        _multiplayerGameState = multiplayerGameState;
        _multiplayerGameState->retain();
        _multiplayerGameState->listenForIncomingPackets();
        
        // Get the screen size
        _screenSize = CCDirector::sharedDirector()->getWinSize();
        
        _gameLayer = NULL;
        
        _finishLineScreenXPosition = 0;
        _startLineScreenXPosition = 0;
        _heroLastKnownScreenXPosition = 0;
        _viewPointX = 0;
        _heroLastCalculatedForScoreWorldXPosition = 0;
        _score = 0;
        _timeSpentRunningInSeconds = 0;
        _timeSinceLastScoreReport = 0;
        _multiplayerUpdateSentCounter = 0.0f;
        _startGameCounter = 0;
        
        _player = NULL;
        _gameLayer = NULL;
        _powerUpBoxes = NULL;
        _collectedPowerUp = NULL;
        _hudLayer = NULL;
        _loadingLayer = NULL;
        
        _heroFireBalls = CCArray::create();
        _heroFireBalls->retain();
        
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                      callfuncO_selector(GameLayer::multiplayerGameStateReady),
                                                                      MULTIPLAYER_NOTIFICATION_OPPONENTS_SPRITES_READY,
                                                                      NULL);
        
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                      callfuncO_selector(GameLayer::readyToStartGameCallback),
                                                                      NEXTPEER_NOTIFICATION_RECEIVE_SYNCHRONIZED_EVENT,
                                                                      NULL);
        
        loadGameLoadState();
        
        //create main loop
        this->schedule(schedule_selector(GameLayer::update));
        
        //listen for touches
        this->setTouchEnabled(true);
        
		bRet = true;
	} while (0);
    
	return bRet;
}

void GameLayer::update(float dt)
{
    // Update the game clock only when the game has started for all connected players (post sync event)
    if (_state == kGamePlay || _state == kGameStartCountDown) {
        GameClock::getInstance()->update(dt);
    }
    
    switch (_state) {
        case kGamePlay: {
            
            updatePlayState(dt);
            break;
        }
        default: {
            break;
        }
    }
}

void GameLayer::updatePlayState(float dt)
{
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    _world->Step(dt, velocityIterations, positionIterations);
    
    _player->updateJumpability((_contactListener -> getFootContacts() > 0));
    
    _player->update(dt);
    
    CCPoint heroScreenPosition = _player->getPosition();
    
    // Move the game layer by the hero movement in the level, adjust the HUD layer by the game's layer position
    CCPoint gameLayerFuturePosition = ccp(this->getPositionX(), 0);
    CCPoint hudPosition = _hudLayer->getPosition();
    
    // Layer X movement
    // Moving the screen as long as we have more of the world to see
    float screenMovementX = (_heroLastKnownScreenXPosition - heroScreenPosition.x);
    if ((this->getPositionX() > _worldWidthBoundaryInScreenPoints) && (heroScreenPosition.x > _viewPointX)) {
        // Moving the screen and the static HUD items (The HUD items are located on the current layer)
        gameLayerFuturePosition.x = this->getPositionX() + screenMovementX;
        hudPosition.x = hudPosition.x - screenMovementX;
    }
    _heroLastKnownScreenXPosition = heroScreenPosition.x;
    
    // Layer Y Movement - Follow to player in bounds (follow movement up, not down)
    float gameLayerPositionYShift = _screenSize.height -_player->getContentSize().height - heroScreenPosition.y;
    if (gameLayerPositionYShift < 0) {
        gameLayerFuturePosition.y = gameLayerPositionYShift;
    }
    else {
        gameLayerFuturePosition.y = 0;
    }
    
    // Shift the HUD layer by the game's layer movement (so they will be keep the original position).
    float adjustedYChange = abs(gameLayerFuturePosition.y) - abs(getPositionY());
    setPosition(gameLayerFuturePosition);
    
    hudPosition.y += adjustedYChange;
    _hudLayer->setPosition(hudPosition);
    
    ViewPort::getInstance()->setScreenStartXPosition(this->getPositionX());
    
    // Update opponents
    CCArray* opponents = _multiplayerGameState->getAllOpponents();
    if (opponents) {
        unsigned int count = opponents->count();
        for (unsigned int i = 0; i < count; i++) {
            Opponent* opponent = (Opponent*)opponents->objectAtIndex(i);
            opponent->update(dt);
        }
    }
    
    // Update opponents fire balls movements
    CCArray* opponentFireBalls = _multiplayerGameState->getAllOpponentFireBalls();
    if (opponentFireBalls) {
        
        // Update the fire balls
        CCObject* it = NULL;
        
        bool shouldRemoveFireBall = false;
        CCARRAY_FOREACH_REVERSE(opponentFireBalls, it)
        {
            OpponentFireBall *fireBall = static_cast<OpponentFireBall*>(it);
            
            // If the fireball is not visible, don't try to calculate the hit -> skip
            if (!fireBall->isVisible()) {
                continue;
            }
            shouldRemoveFireBall = false;
            
            // If the fire fireball has timed out, don't bother update it
            if (fireBall->hasTimedOut()) {
                shouldRemoveFireBall = true;
            }
            else {
                // Update the fire ball
                fireBall->update(dt);
                
                // If the hero has no shield and the fire ball hit the hero, notify the hero object (state change, animation change)
                if (_player -> getHeroPowerUpState() != kHeroPowerUpStateShield && fireBall->isHit(_player)) {
                    
                    // This will change the hero state as well as applying animation on the player
                    _player->hitByFireBall();
                    shouldRemoveFireBall = true;
                }
                // Else, check hit on other opponents (if there are any) opponent fire ball on another opponent
                else if (opponents) {
                    CCObject* it = NULL;
                    CCARRAY_FOREACH(opponents, it)
                    {
                        Opponent *opponent = static_cast<Opponent*>(it);
                        
                        // Skip this opponent has a shield
                        if (opponent->getOpponentPowerUpState() == kOpponentPowerUpStateShield) {
                            continue;
                        }
                        
                        // Opponent cannot hit itself.
                        if ( opponent->getPlayerData()->getPlayerId() == fireBall->getFromOpponentId()) {
                            continue;
                        }
                        
                        // Each fire ball can hit only one opponent
                        if (fireBall->isHit(opponent)) {
                            
                            opponent->simulateHitByFireBallIfRequired();
                            
                            shouldRemoveFireBall = true;
                            
                            // Can only hit one opponent
                            break;
                        }
                    }
                }
            }
            
            // If the fire ball should be removed, mark it as invisible, and remove from the parent & game state
            if (shouldRemoveFireBall || (fireBall->getFireBallState() == kFireBallStopped)) {
                
                // This will make the fire ball will not visible any more
                fireBall->setVisible(false);
                fireBall->removeFromParent();
                
                fireBall->markAsStopped(); // Make sure it marked as stopped (it can be that it has timedout)
            }
            // Else (not stopped fire ball) Add the fireball and advance it
            else if (fireBall->getParent() == NULL) {
                _charactersBatchNode->addChild(fireBall);
            }
        }
        
        // Trim the fire balls which are not valid any more (timed out & invalid)
        _multiplayerGameState->trimOpponentFireBalls();
    }
    
    // Handling the hero states
    HeroState heroState = _player->getHeroState();
    if (heroState == kHeroReady) {
        _player->startRace();
    }
    // Else, moving -> update the scores & check if we reached the finish line
    else if (heroState == kHeroRunning) {
        
        // Calculate the time from the start line ahead
        if (heroScreenPosition.x > _startLineScreenXPosition) {
            _timeSpentRunningInSeconds += dt;
        }
        
        bool isPlayerStuck = _player->isStuck();
        
        if (!isPlayerStuck) {
            
            b2Vec2 heroWorldPosition = _player->getB2Body()->GetPosition();
            
            // For the ongoing score reporting we will calculate the distance that the player has passed in the world since the last update.
            if (heroScreenPosition.x > _startLineScreenXPosition) {
                float distance = (heroWorldPosition.x - _heroLastCalculatedForScoreWorldXPosition);
                
                if (distance > 0) {
                    // The score is a multiplayer of the passed distance since last update and the score modifier;
                    _score += distance*PLAYER_SCORE_DISTANCE_MODIFIER;
                    this->updateScoreLabel();
                }
            }
            
            _heroLastCalculatedForScoreWorldXPosition = heroWorldPosition.x;
            
            // If the player is moving, check if it reached the finish line already
            if (heroScreenPosition.x >= _finishLineScreenXPosition) {
                _player->passedFinishLine();
                
                // calculate bonus score based on how long it took the player to complete the level
                int bonus = MAX(0, LEVEL_MAX_TIME - _timeSpentRunningInSeconds) * PLAYER_BONUS_PER_SECOND;
                
                // If the player finished with a bonus, show the bonus on the screen
                if (bonus > 0) {
                    _score += bonus;
                    this->updateScoreLabel();
                    showBonusLabel(bonus);
                }
            }
            // Else, (the player is not stuck and have not reached the finish line), in case the player has not any power up, check if there are any random boxes left.
            else if (_collectedPowerUp == NULL && _powerUpBoxes != NULL && _powerUpBoxes->count() > 0) {
                PowerUp *powerUp = static_cast<PowerUp*>(_powerUpBoxes->objectAtIndex(0));
                
                // Hit test the upcoming power up with the hero figure.
                if (powerUp->isHit(_player)) {
                    
                    _collectedPowerUp = powerUp;
                    _collectedPowerUp->retain();
                    
                    // Hide the power up (we not going to remove this power up for the other players, just show the user that he/she took the power up).
                    _collectedPowerUp->getPowerUpNode()->setVisible(false);
                    
                    // Remove the reference
                    _powerUpBoxes->removeObjectAtIndex(0);
                    
                    // In case the user collected a power up, show the right power up button in the HUD
                    switch (_collectedPowerUp->getPowerUpType()) {
                        case kPowerUpExtraSpeed: {
                            _usePowerNode->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("use_speed_button.png"));
                            _usePowerHudMenu->setVisible(true);
                        }
                            break;
                        case kPowerUpShield: {
                            _usePowerNode->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("use_shield_button.png"));
                            _usePowerHudMenu->setVisible(true);
                        }
                            break;
                        case kPowerUpFireBall: {
                            _usePowerNode->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("use_fire_button.png"));
                            _usePowerHudMenu->setVisible(true);
                        }
                            break;
                            
                        default: {
                            // Could not find this power up, remove the power up.
                            CC_SAFE_RELEASE_NULL(_collectedPowerUp);
                        }
                            break;
                    }
                    
                }
                // The screen position passed that powerup -> remove it
                else if (abs(getPositionX()) > powerUp->getPowerUpPosition().x){
                    _powerUpBoxes->removeObjectAtIndex(0);
                }
            }
        }
    }
    // Hero stopped. End the race with delay (so the player will see the bonus)
    else if (heroState == kHeroStopped) {
        CCCallFunc *finishEndRace = CCCallFunc::create(this, callfunc_selector(GameLayer::callbackDoneEndRaceAnimation));
        CCDelayTime *finishRaceDelay = CCDelayTime::create(3.0);
        CCSequence* finishRaceSeq = CCSequence::create(finishRaceDelay, finishEndRace, NULL);
        this->runAction(finishRaceSeq);
    }
    
    // Update the hero fire balls if exist (just advancing them in the world, remove them if they hit any opponent)
    if (_heroFireBalls != NULL) {
        
        CCObject* it = NULL;
        unsigned int heroFireBallsIndex = _heroFireBalls->count() - 1;
        CCARRAY_FOREACH_REVERSE(_heroFireBalls, it)
        {
            HeroFireBall *fireBall = static_cast<HeroFireBall*>(it);
            
            fireBall->update(dt);
            _multiplayerGameState->dispatchUpdateForFireBall(fireBall);
            
            bool hasFireBallStopped = (fireBall->getFireBallState() == kFireBallStopped);
            
            // If we still have a fire ball (going on). Check if hte fire ball  hit any of the opponents
            if (!hasFireBallStopped && opponents) {
                CCObject* it = NULL;
                CCARRAY_FOREACH(opponents, it)
                {
                    Opponent *opponent = static_cast<Opponent*>(it);
                    
                    // Skip this opponent has a shield
                    if (opponent->getOpponentPowerUpState() == kOpponentPowerUpStateShield) {
                        continue;
                    }
                    
                    // Each fire ball can hit only one opponent
                    if (fireBall->isHit(opponent)) {
                        opponent->simulateHitByFireBallIfRequired();
                        hasFireBallStopped = true;
                        
                        // Can only hit one opponent
                        break;
                    }
                }
            }
            
            // If the fire ball has stopped, remove it from the rendering loop
            if (hasFireBallStopped) {
                fireBall->setVisible(false);
                fireBall->removeFromParent();
                _heroFireBalls->removeObjectAtIndex(heroFireBallsIndex);
            }
            heroFireBallsIndex--;
        }
    }
    
    // Send an update about our player if needed
    if (_multiplayerUpdateSentCounter % 2 == 0) {
        _multiplayerGameState->dispatchUpdateForHero(_player);
    }
    _multiplayerUpdateSentCounter++;
    
    // Send a score update every 1 second
    _timeSinceLastScoreReport += dt;
    if (_timeSinceLastScoreReport > 1.0f) {
        CCNextpeer::getInstance()->reportScoreForCurrentTournament((unsigned int)_score);
        _timeSinceLastScoreReport = 0.0f;
    }
    
}

void GameLayer::startGameCountDown(float dt) {
    CCNode *currentCountDown = NULL;
    if (_startGameCounter == 0){
        
        CCNotificationCenter::sharedNotificationCenter()->postNotification(MULTIPLAYER_NOTIFICATION_SHOULD_START_GAME);
        currentCountDown = CCSprite::createWithSpriteFrameName("hud_wait_go.png");
        this->unschedule(schedule_selector(GameLayer::startGameCountDown));
        _state = kGamePlay;
    }
    else if (_startGameCounter <= MAX_WAIT_FOR_GAMES_IMAGES){
        char szValue[100] = {0};
        sprintf(szValue, "hud_wait_%i.png", (int) _startGameCounter);
        currentCountDown = CCSprite::createWithSpriteFrameName(szValue);
        _startGameCounter--;
    }
    
    if (currentCountDown == NULL) return;
    
    currentCountDown->setVisible(false);
    _hudBatchNode->addChild(currentCountDown);
    
    CCFiniteTimeAction* action = CCSequence::create(
                                                    CCPlace::create(ccp(_screenSize.width/2, _screenSize.height/2)),
                                                    CCShow::create(),
                                                    CCSpawn::create(CCScaleBy::create(1.0, 1.5), CCFadeOut::create(1.0), NULL),
                                                    CCCallFuncN::create(this, callfuncN_selector(GameLayer::callbackstartGameCountDownAnimation)),
                                                    NULL);
    
    currentCountDown->runAction(action);
}

void GameLayer::callbackstartGameCountDownAnimation(CCNode* sender) {
    _gameLayer->removeChild(sender);
}

void GameLayer::loadGameLoadState() {
    _loadingLayer = CCLayerColor::create(ccc4(0, 0, 0, 255));
    _loadingLayer->retain();
    _loadingLayer->setContentSize(_screenSize);
    CCLabelTTF *loadingLevelTitle = CCLabelTTF::create("Loading...", TTF_FONT_NAME, TTF_LOADING_TITLE_FONT_SIZE);
    loadingLevelTitle->setPosition(ccp(_screenSize.width/2, _screenSize.height/2));
    _loadingLayer->addChild(loadingLevelTitle);
    
    this->addChild(_loadingLayer);
    
    _state = kGameLoad;
    
    // We loading the level asset. Start after the loading layer has been loaded.
    this->scheduleOnce(schedule_selector(GameLayer::loadLevelAssets), 0.1);
}

void GameLayer::loadLevelAssets() {
    
    // Create the game layer but hide it at first (we will display it once the level has loaded)
    _gameLayer = CCLayer::create();
    _gameLayer->retain();
    _gameLayer->setContentSize(_screenSize);
    _gameLayer->setVisible(false);
    
    _hudLayer = CCLayer::create();
    _hudLayer->retain();
    _hudLayer->setContentSize(_gameLayer->getContentSize());
    
    // Create the Box2D world
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    _world = new b2World(gravity);
    
    _multiplayerGameState -> setWorld(_world);
    
    _contactListener = new GameContactListener();
    _world->SetContactListener(_contactListener);
    
    // Add the atlas to the hud layer
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("InGame.plist");
    _hudBatchNode = CCSpriteBatchNode::create("InGame.png", 200);
    _hudLayer->addChild(_hudBatchNode);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Characters.plist");
    _charactersBatchNode = CCSpriteBatchNode::create("Characters.png", 200);
    _gameLayer->addChild(_charactersBatchNode, kGameLayerZOrderDefault);
    
    // Create the hero
    _player = Hero::create(_world);
    _player->retain();
    
    // Ask the game level to give us a random level (all connected players will have the same level as the class use Nextpeer's random function)
    _loader = GameLevel::getRandomLevelLoader();
    if(!_loader->isGravityZero()){
        _loader->createGravity(_world);
    }
    
    // Register our selector to the loading progress. Once the level has finished loading we will switch to the next game step
    _loader->registerLoadingProgressObserver(this, schedule_selector(GameLayer::levelLoadingProgress));
    
    _loader->addObjectsToWorld(_world, _gameLayer);
    _loader->createPhysicBoundaries(_world);
    
    // Send out the avatar notification
    _multiplayerGameState->dispatchAvatarNotification(*_player->getPlayerData());
    
    _multiplayerGameState->setCanLoadOpponents(true);
    _multiplayerGameState->flushOpponentsWaitingToLoadQueue();
    
    // Extracting the world size from the level loader
    _worldSize = _loader->gameWorldSize().size;
    
    // Set the view point on the screen
    _worldWidthBoundaryInScreenPoints = -(_worldSize.width - _screenSize.width);
    _viewPointX = ccpMult(_screenSize, 0.3f).x;
}

void GameLayer::levelLoadingProgress(float val) {
    if (_loadingLayer == NULL) return;
    
    // When level is loaded (1.0 represents that the level is fully loaded), remove the loading layer and add the game layer
    // Start the waiting for other players state
    if (val == 1.0f) {
        // Unregister the level loading
        _loader->registerLoadingProgressObserver(NULL, NULL);
        
        // This will make sure that the hud will be the top most layer
        _gameLayer->addChild(_hudLayer, kGameLayerZOrderHUD);
        _gameLayer->setVisible(true);
        _loadingLayer->setVisible(false);
        removeChild(_loadingLayer);
        CC_SAFE_RELEASE_NULL(_loadingLayer);
        
        this->addChild(_gameLayer);
        _gameLayer->setVisible(true);
        loadWaitingForOtherPlayersState();
    }
}

void GameLayer::loadWaitingForOtherPlayersState() {
    _state = kGameWaitForOtherPlayers;
    
    _waitForPlayersNode = CCSprite::createWithSpriteFrameName("hud_waiting_for_players.png");
    _waitForPlayersNode->setPosition(ccp(_screenSize.width/2, _screenSize.height/2));
    _waitForPlayersNode->setVisible(true);
    
    _waitForPlayesActivityIndicatorNode = CCSprite::createWithSpriteFrameName("hud_activity_indicator.png");
    _waitForPlayesActivityIndicatorNode->setPosition(ccp(_screenSize.width/2, _waitForPlayersNode->getPositionY() - _waitForPlayersNode->getContentSize().height/2));
    _waitForPlayesActivityIndicatorNode->runAction(CCRepeatForever::create(CCRotateBy::create(0.05, 10.0)));
    
    _hudBatchNode->addChild(_waitForPlayersNode);
    _hudBatchNode->addChild(_waitForPlayesActivityIndicatorNode);
    
    loadHUDElements();
    
    // Place a timeout on the loading for other players state, in case a player will chose to cancel the game.
    this->scheduleOnce(schedule_selector(GameLayer::waitForOpponentsSpritesLoadingTimeout), MAX_WAIT_FOR_OPPONENT_SPRITES_SIGNAL_SECONDS);
}

void GameLayer::loadHUDElements() {
    CCNode* hudX = CCSprite::createWithSpriteFrameName("hud_x.png");
    CCMenuItemSprite* endGame = CCMenuItemSprite::create(hudX, hudX, this, menu_selector(GameLayer::menuCallbackEndGame));
    CCMenu *leaveGameHudMenu = CCMenu::create(endGame, NULL);
    leaveGameHudMenu->setPosition(ccp(_screenSize.width - hudX->getContentSize().width/2 - HUD_ITEMS_SPACING, _screenSize.height - hudX->getContentSize().height/2 - HUD_ITEMS_SPACING));
    _hudLayer->addChild(leaveGameHudMenu);
    
    CCNode* jumpButtonNode = CCSprite::createWithSpriteFrameName("jump_button.png");
    CCMenuItemSprite* jumpButton = CCMenuItemSprite::create(jumpButtonNode, jumpButtonNode, this, menu_selector(GameLayer::menuCallbackJump));
    CCMenu *jumpHudMenu = CCMenu::create(jumpButton, NULL);
    jumpHudMenu->setPosition(ccp(_screenSize.width - jumpButtonNode->getContentSize().width/2 - HUD_ITEMS_SPACING, jumpButtonNode->getContentSize().height/2 + HUD_ITEMS_SPACING));
    _hudLayer->addChild(jumpHudMenu);
    
    _usePowerNode = CCSprite::createWithSpriteFrameName("use_fire_button.png");
    CCMenuItemSprite* usePowerButton = CCMenuItemSprite::create(_usePowerNode, _usePowerNode, this, menu_selector(GameLayer::menuCallbackUsePower));
    _usePowerHudMenu = CCMenu::create(usePowerButton, NULL);
    _usePowerHudMenu->setPosition(ccp(_usePowerNode->getContentSize().width/2 + HUD_ITEMS_SPACING,  _usePowerNode->getContentSize().height/2 + HUD_ITEMS_SPACING));
    _usePowerHudMenu->setVisible(false); // We will enable it once the player got a power
    _hudLayer->addChild(_usePowerHudMenu);
    
    // Position the score display next to the "X" menu item
    _scoreDisplay = CCLabelBMFont::create("0", BM_FONT_NAME, _screenSize.width * 0.4f, kCCTextAlignmentCenter);
    _scoreDisplay->setAnchorPoint(ccp(1,1));
    _scoreDisplay->setPosition(ccp(leaveGameHudMenu->getPositionX() - hudX->getContentSize().width/2 - HUD_ITEMS_SPACING, leaveGameHudMenu->getPositionY() + _scoreDisplay->getContentSize().height/2));
    
    _hudLayer->addChild(_scoreDisplay);
}

void GameLayer::loadPlayState()
{
    locateRandomBoxes();
    locateStartLine();
    preparePlayersForRace(locateFinishLine());
}

float GameLayer::locateStartLine() {
    // Looking for the start sign, getting its screen X position.
    CCArray * sprites = _loader->spritesWithTag(GAME_LEVEL_START_FLAG);
    if (sprites -> count() > 0) {
        CCNode* sprite = (CCNode*)sprites->objectAtIndex(0);
        _startLineScreenXPosition = sprite->getPositionX();
    }
    // Error
    else {
        CCLog("Could not find the start line position");
        _startLineScreenXPosition = 0;
    }
    
    return _startLineScreenXPosition;
}

float GameLayer::locateFinishLine() {
    // Looking for the end sign, getting its screen X position.
    CCArray *sprites = _loader->spritesWithTag(GAME_LEVEL_END_FLAG);
    if (sprites -> count() > 0) {
        CCNode* sprite = (CCNode*)sprites->objectAtIndex(0);
        _finishLineScreenXPosition = sprite->getPositionX();
    }
    // Error, this will stop the level on the spot.
    else {
        CCLog("Could not find the finish line position");
        _finishLineScreenXPosition = 0;
    }
    
    return _finishLineScreenXPosition;
}

void GameLayer::locateRandomBoxes() {
    
    _powerUpBoxes = NULL;
    
    // Locate all powerup boxes on the level
    CCObject* it = NULL;
    
    // Fetch the power up boxes from the current level by their tag.
    CCArray* randomBoxesSprites = _loader->spritesWithTag(GAME_LEVEL_POWERUP_BOX);
    if (randomBoxesSprites != NULL && randomBoxesSprites->count() > 0) {
        _powerUpBoxes = CCArray::createWithCapacity(randomBoxesSprites->count());
        _powerUpBoxes->retain();
        
        // Sort the random powerup boxes by their location on the screen (first to last - screen X position)
        CCSortableArray* sortedBoxesSprites = CCSortableArray::createWithArray(randomBoxesSprites);
        sortedBoxesSprites->sort(GameLayer::powerUpBoxScreenPositionNodeComparator);
        
        it = NULL;
        uint32_t powerUpId = 0;
        CCARRAY_FOREACH(sortedBoxesSprites, it)
        {
            CCNode *sprite = static_cast<CCNode*>(it);
            PowerUp *powerUp = PowerUp::create(powerUpId, sprite, PowerUp::randomizePowerUpType());
            _powerUpBoxes->addObject(powerUp);
            
            // Advance the sequence to the next power up.
            powerUpId++;
        }
    }
    
}

void GameLayer::preparePlayersForRace(float startLineScreenXPosition)
{
    CCArray* players = NULL;
    
    // Create an array of the opponents + the current player
    CCArray* opponents = _multiplayerGameState->getAllOpponents();
    if (opponents == NULL) {
        players = CCArray::createWithObject(_player);
    }
    else {
        players = CCArray::createWithCapacity(opponents->count() + 1);
        players->initWithArray(opponents);
        players->addObject(_player);
    }
    
    float terrainHeight = 0;
    
    // Looking for the top brick, getting its screen top Y position so we'll place the players at the right level.
    CCArray* sprites = _loader->spritesWithTag(GAME_LEVEL_START_BLOCK);
    if (sprites -> count() > 0) {
        CCNode* sprite = (CCNode*)sprites->objectAtIndex(0);
        terrainHeight = sprite->getPositionY() + sprite->getContentSize().height/2;
    }
    // Error, report to console, use the screen height as a marker
    else {
        CCLog("Could not find the start top brick position");
        terrainHeight = _screenSize.height/2.0f;
    }
    
    
    unsigned int playerCount = players->count();
    
    // Make sure the array is sorted by id, this way we ensure all devices use the same start arrangement
    CCSortableArray* sortedPlayers = CCSortableArray::createWithArray(players);
    sortedPlayers->sort(GameLayer::playerIdComparator);
    
    for (unsigned int i = 0; i < playerCount; i++) {
        unsigned int rand1 = Rand::generate(0, playerCount);
        unsigned int rand2 = Rand::generate(0, playerCount);
        
        if (rand1 != rand2) {
            sortedPlayers->exchangeObjectAtIndex(rand1, rand2);
        }
    }
    
    // Now that our array has been shuffled, we'll generate world positions for all the players
    ViewPort *viewPort = ViewPort::getInstance();
    
    // Figure out how much distance we have to place the players (from 0..start line) in world positionm
    float startLineInWorldPosition = viewPort->screenToWorldCoordinate(ccp(startLineScreenXPosition, 0)).x;
    
    // Each player has this amount of world width space in the start position (take an extra slot free ahead - we going to use it)
    float eachPlayerWorldPositionSlot = startLineInWorldPosition/(playerCount + 1);
    float playerSpaceScreenSlot = viewPort->worldToScreenCoordinate(b2Vec2(eachPlayerWorldPositionSlot, 0)).x;
    
    // The slot will be the smallest between the last player content size or the slot.
    // We use the last player as it is arranged the same for all of the players.
    Player* lastPlayer = static_cast<Player*>((sortedPlayers)->lastObject());
    playerSpaceScreenSlot = min(playerSpaceScreenSlot, lastPlayer->getContentSize().width);
    
    CCObject* currentElem = NULL;
    int playerCounter = 0;
    CCARRAY_FOREACH(sortedPlayers, currentElem)
    {
        Player* player = static_cast<Player*>(currentElem);
        
        float groundPosScreenPositionY = terrainHeight + player->getContentSize().height;
        player->setVisible(false);
        
        // In the middle of each slot (regardless of the player size, that will better fit smaller devices with a lot of players).
        // Each player will be arranged in the next slot (so the player will not be near world edge.
        CCPoint screenPos = ccp((1+ playerCounter)*playerSpaceScreenSlot + playerSpaceScreenSlot/2, groundPosScreenPositionY);
        
        // Setting the opponent's world position so when there will be a position pre-network update
        if (player != _player)  {
            static_cast<Opponent*>(player)->setPosition(screenPos);
        }
        else {
            // Set player's first position
            _heroLastKnownScreenXPosition = screenPos.x;
        }
        
        // Set the start screen coordinates for each player
        player->setPosition(screenPos);
        
        // Add the player to the batch node
        _charactersBatchNode->addChild(player);
        
        player->setVisible(true);
        
        playerCounter++;
    }
}

void GameLayer::updateScoreLabel()
{
    char szValue[100] = {0};
    sprintf(szValue, "%i", (int) _score);
    _scoreDisplay->setString (szValue);
}

void GameLayer::showBonusLabel(int bonus) {
    
    char szValue[100] = {0};
    sprintf(szValue, "%i", bonus);
    CCLabelBMFont *bonusSize = CCLabelBMFont::create(szValue, BM_FONT_NAME, _screenSize.width * 0.4f, kCCTextAlignmentCenter);
    CCLabelTTF *bonusTitle = CCLabelTTF::create("Bonus!", TTF_FONT_NAME, TTF_BONUS_TITLE_FONT_SIZE);
    
    bonusTitle->setPosition(ccp(_screenSize.width*0.5, _screenSize.height*0.5 + bonusTitle->getContentSize().height));
    bonusTitle->setOpacity(0);
    _hudLayer->addChild(bonusTitle);
    
    bonusSize->setPosition(ccp(bonusTitle->getPositionX(), bonusTitle->getPositionY() - bonusTitle->getContentSize().height));
    bonusSize->setOpacity(0);
    _hudLayer->addChild(bonusSize);
    
    bonusTitle->runAction(CCFadeIn::create(0.5f));
    bonusSize->runAction(CCFadeIn::create(0.5f));
}

void GameLayer::dispatchFireBall(uint32_t fromPowerUpId) {
    
    // Invalid state (we don't have any random boxes -> should not be any fire balls in the game)
    if (_heroFireBalls == NULL) return;
    
    CCPoint playerPosition = _player->getPosition();
    CCSize playerContentSize = _player->getContentSize();
    CCPoint firePoint =  ccp(playerContentSize.width + playerPosition.x, playerPosition.y - playerContentSize.height/2);
    
    HeroFireBall *fireBall = HeroFireBall::create(firePoint, fromPowerUpId, _world);
    fireBall->setPosition(firePoint);
    _heroFireBalls->addObject(fireBall);
    
    // Add the player to the batch node
    _charactersBatchNode->addChild(fireBall);
}

void GameLayer::ccTouchesBegan(CCSet* pTouches, CCEvent* event) {
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    
    if (touch) {
        menuCallbackJump(this);
    }
}

void GameLayer::menuCallbackUsePower(CCObject* sender) {
    if (_collectedPowerUp == NULL) return;
    
    // The player can use power ups only while in game
    if (_player->getHeroState() != kHeroRunning) return;
    
    // The player is hurt, can't use a power-up while hurt
    if (_player->isHurt()) return;
    
    PowerUpType powerUpType = _collectedPowerUp->getPowerUpType();
    
    // The player is using a power up at the moment, can only fire while using power up (ignore the rest).
    if (powerUpType != kPowerUpFireBall && _player->getHeroPowerUpState() != kHeroPowerUpStateNone) return;
    
    
    switch (powerUpType) {
        case kPowerUpExtraSpeed: {
            
            _player->useSpeedBoostPowerUp();
        }
            break;
        case kPowerUpShield: {
            _player->useShieldPowerUp();
        }
            break;
        case kPowerUpFireBall: {
            dispatchFireBall(_collectedPowerUp->getPowerUpId());
        }
            break;
            
        default:
            break;
    }
    
    CC_SAFE_RELEASE_NULL(_collectedPowerUp);
    
    // Hide the use power node
    _usePowerHudMenu->setVisible(false);
}

void GameLayer::callbackDoneEndRaceAnimation()
{
    CCNextpeer::getInstance()->reportControlledTournamentOverWithScore((int)_score);
}

void GameLayer::menuCallbackEndGame(CCObject* sender) {
    
    CCNextpeer::getInstance()->reportForfeitForCurrentTournament();
    
    // Switch back to the main menu
    CCDirector::sharedDirector()->replaceScene(MainMenuLayer::scene());
}

void GameLayer::menuCallbackJump(CCObject* sender) {
    switch (_state) {
        case kGamePlay: {
            
            _player->jump();
            break;
        }
        default: {
            break;
        }
    }
}

bool GameLayer::playerIdComparator(CCObject *player1, CCObject *player2)
{
    Player* pPlayer1 = static_cast<Player*>(player1);
    Player* pPlayer2 = static_cast<Player*>(player2);
    
    int result = pPlayer1->getPlayerData()->getPlayerId().compare(pPlayer2->getPlayerData()->getPlayerId());
    
    return result < 0;
}

bool GameLayer::powerUpBoxScreenPositionNodeComparator(CCObject *box1, CCObject *box2) {
    CCNode* pBox1 = static_cast<CCNode*>(box1);
    CCNode* pBox2 = static_cast<CCNode*>(box2);
    return (pBox1 -> getPositionX() < pBox2 -> getPositionX());
}

void GameLayer::multiplayerGameStateReady(CCObject *pSender)
{
    // Remove the wait for opponents sprite loading timeout selector (as those already loaded)
    this->unschedule(schedule_selector(GameLayer::waitForOpponentsSpritesLoadingTimeout));
    
    // Register to a synchronized event for this game
    CCNextpeer::getInstance()->registerToSynchronizedEvent(kStartGameSyncEventName, kStartGameSyncEventTimeout);
}

void GameLayer::readyToStartGameCallback(CCObject *pSender)
{
    if (_state == kGameWaitForOtherPlayers) {
        _state = kGameStartCountDown;
        
        // Reset the game clock (the countdown is about to start on all connected devices)
        GameClock::getInstance()->reset();
        
        _waitForPlayersNode->setVisible(false);
        _waitForPlayesActivityIndicatorNode->setVisible(false);
        _hudLayer->removeChild(_waitForPlayersNode);
        _hudLayer->removeChild(_waitForPlayesActivityIndicatorNode);
        
        this->loadPlayState();
        
        _startGameCounter = PRE_GAME_COUNT_DOWN_SECONDS;
        this->schedule(schedule_selector(GameLayer::startGameCountDown), 1.0f);
    }
}

void GameLayer::waitForOpponentsSpritesLoadingTimeout(float dt)
{
    _multiplayerGameState->waitForOpponentsSpritesTimedOut();
}
