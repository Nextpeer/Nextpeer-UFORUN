//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "GameLayer.h"

#include "Hero.h"
#include "ViewPort.h"
#include "GameContactListener.h"
#include "PowerUp.h"
#include "CCSortableArray.h"
#include "MainMenuLayer.h"
#include "HeroFireBall.h"
#include "CCNextpeer.h"

using namespace nextpeer;

#define HUD_ITEMS_SPACING 10.0f

#define BM_FONT_NAME "font.fnt"
#define TTF_FONT_NAME "Thonburi"
#define TTF_BONUS_TITLE_FONT_SIZE 30

// The maximum time in the level that will yield a bonus
#define LEVEL_MAX_TIME 100.0f

typedef enum
{
    kGameLayerZOrderDefault = 10,
    kGameLayerZOrderHUD = 20
    
} GameLayerZOrder;

CCScene* GameLayer::scene()
{
	CCScene * scene = NULL;
	do
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
        
		// 'layer' is an autorelease object
        GameLayer *layer = GameLayer::create();
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
    
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_gameLayer);
    CC_SAFE_RELEASE_NULL(_hudLayer);
    CC_SAFE_RELEASE_NULL(_collectedPowerUp);
    CC_SAFE_RELEASE_NULL(_heroFireBalls);
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
	bool bRet = false;
	do
	{
        //////////////////////////////
        // 1. super init first
        CC_BREAK_IF(! CCLayer::init());
        
        _gameLayer = NULL;
        
        _finishLineScreenXPosition = 0;
        _startLineScreenXPosition = 0;
        _heroLastKnownScreenXPosition = 0;
        _viewPointX = 0;
        _heroLastCalculatedForScoreWorldXPosition = 0;
        _score = 0;
        _timeSpentRunningInSeconds = 0;
        _timeSinceLastScoreReport = 0;
        
        _player = NULL;
        _gameLayer = NULL;
        _powerUpBoxes = NULL;
        _collectedPowerUp = NULL;
        _hudLayer = NULL;
        _heroFireBalls = NULL;
        
        loadLevelAssets();
        
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
    if (_state != kGamePlay) return;
    
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
            
            // If the fire ball has stopped, remove it from the rendering loop
            if ((fireBall->getFireBallState() == kFireBallStopped)) {
                fireBall->setVisible(false);
                fireBall->removeFromParent();
                _heroFireBalls->removeObjectAtIndex(heroFireBallsIndex);
            }
            heroFireBallsIndex--;
        }
    }
    
    // Send a score update every 1 second
    _timeSinceLastScoreReport += dt;
    if (_timeSinceLastScoreReport > 1.0f) {
        CCNextpeer::getInstance()->reportScoreForCurrentTournament((unsigned int)_score);
        _timeSinceLastScoreReport = 0.0f;
    }
}

void GameLayer::loadLevelAssets() {
    
    _state = kGameLoad;
    
    // Get the screen size
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    _gameLayer = CCLayer::create();
    _gameLayer->retain();
    _gameLayer->setContentSize(_screenSize);
    
    _hudLayer = CCLayer::create();
    _hudLayer->retain();
    _hudLayer->setContentSize(_gameLayer->getContentSize());
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    _world = new b2World(gravity);
    
    _contactListener = new GameContactListener();
    _world->SetContactListener(_contactListener);
    
    // Load the level for the game
    _loader = new LevelHelperLoader("level01.plhs");

    if(!_loader->isGravityZero()){
        _loader->createGravity(_world);
    }
    
    _loader->addObjectsToWorld(_world, _gameLayer);
    _loader->createPhysicBoundaries(_world);
    
    // Extracting the world size from the level loader
    _worldSize = _loader->gameWorldSize().size;
    _worldWidthBoundaryInScreenPoints = -(_worldSize.width - _screenSize.width);
    _viewPointX = ccpMult(_screenSize, 0.3f).x;
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Characters.plist");
    _charactersBatchNode = CCSpriteBatchNode::create("Characters.png", 200);
    _gameLayer->addChild(_charactersBatchNode, kGameLayerZOrderDefault);
    
    // Create the hero, place it under zero point -> it will be arranged later on.
    _player = Hero::create(_world);
    _player->retain();
    
    this->addChild(_gameLayer);
    
    preparePlayersForRace(locateStartLine());
    locateFinishLine();
    locateRandomBoxes();
    
    _heroFireBalls = CCArray::create();
    _heroFireBalls->retain();
    
    // Add the atlas to the hud layer
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("InGame.plist");
    _hudBatchNode = CCSpriteBatchNode::create("InGame.png", 200);
    _hudLayer->addChild(_hudBatchNode);
    
    loadHUDElements();
    
    _gameLayer->addChild(_hudLayer, kGameLayerZOrderHUD);
    
    _state = kGamePlay;
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

void GameLayer::preparePlayersForRace(float startLineScreenXPosition)
{
    CCArray* players = CCArray::createWithObject(_player);
    
    float terrainHeight = 0;
    
    // Looking for the top brick, getting its screen top Y position so we'll place the players at the right level.
    CCArray * sprites = _loader->spritesWithTag(GAME_LEVEL_START_BLOCK);
    if (sprites -> count() > 0) {
        CCNode* sprite = (CCNode*)sprites->objectAtIndex(0);
        terrainHeight = sprite->getPositionY() + sprite->getContentSize().height;
    }
    // Error, report to console, use the screen height as a marker
    else {
        CCLog("Could not find the start top brick position");
        terrainHeight = _screenSize.height/2.0f;
    }
    
    unsigned int playerCount = players->count();
    
    // Now that our array has been shuffled, we'll generate world positions for all the players
    ViewPort *viewPort = ViewPort::getInstance();
    
    // Figure out how much distance we have to place the players (from 0..start line) in world positionm
    float startLineInWorldPosition = viewPort->screenToWorldCoordinate(ccp(startLineScreenXPosition, 0)).x;
    
    // Each player has this amount of world width space in the start position (take an extra slot free ahead - we going to use it)
    float eachPlayerWorldPositionSlot = startLineInWorldPosition/(playerCount + 1);
    float playerSpaceScreenSlot = viewPort->worldToScreenCoordinate(b2Vec2(eachPlayerWorldPositionSlot, 0)).x;
    
    // The slot will be the smallest between the last player content size or the slot.
    // We use the last player as it is arranged the same for all of the players.
    Player* lastPlayer = static_cast<Player*>((players)->lastObject());
    playerSpaceScreenSlot = min(playerSpaceScreenSlot, lastPlayer->getContentSize().width);
    
    CCObject* currentElem = NULL;
    int playerCounter = 0;
    CCARRAY_FOREACH(players, currentElem)
    {
        Player* player = static_cast<Player*>(currentElem);
        
        float groundPosScreenPositionY = terrainHeight + player->getContentSize().height;
        player->setVisible(false);
        
        // In the middle of each slot (regardless of the player size, that will better fit smaller devices with a lot of players).
        // Each player will be arranged in the next slot (so the player will not be near world edge.
        CCPoint screenPos = ccp((1+ playerCounter)*playerSpaceScreenSlot + playerSpaceScreenSlot/2, groundPosScreenPositionY);
     
        // Setting the opponent's world position so when there will be a position pre-network update
        if (player != _player)  {
            // TODO: Set world position for opponent
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
    
    if (CCNextpeer::getInstance()->isCurrentlyInTournament()) {
        CCNextpeer::getInstance()->reportForfeitForCurrentTournament();
    }
    
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

bool GameLayer::powerUpBoxScreenPositionNodeComparator(CCObject *box1, CCObject *box2) {
    CCNode* pBox1 = static_cast<CCNode*>(box1);
    CCNode* pBox2 = static_cast<CCNode*>(box2);
    return (pBox1 -> getPositionX() < pBox2 -> getPositionX());
}

