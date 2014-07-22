//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__GameLayer__
#define __UFORun__GameLayer__

#include "cocos2d.h"
USING_NS_CC;
#include "cocos-ext.h"
USING_NS_CC_EXT;

#include "Box2D.h"

#include "LevelHelperLoader.h"

class Hero;
class GameContactListener;
class PowerUp;

typedef enum {
    kGameLoad, // Loading level assets
    kGamePlay // Game play
    
} GameState;

/**
  The game scene layer, Load the level, wait for the other oppoents and render the actual level.
 */
class GameLayer : public CCLayer
{
    private:
    GameState _state;   // The current game state
    CCSize _screenSize; // The actual device's screen size
    CCSize _worldSize;  // The world size (from the loaded level)
    float _worldWidthBoundaryInScreenPoints;   // The boundary for the world (level), which is used for movement calculations (screen points)
    float _startLineScreenXPosition;  // The start line x position (screen points)
    float _finishLineScreenXPosition; // The finish line x position (screen points)
    float _viewPointX; // The view x position on the current world
    float _score; // The hero's current score
    float _timeSpentRunningInSeconds; // The seconds that the player spent actually running (not stuck, from the start line to the finish line)
    float _heroLastKnownScreenXPosition; // The last known screen x position of the player (between updates) -> used to move the screen
    float _heroLastCalculatedForScoreWorldXPosition; // The scene calculates the score as a function of the hero movement in the world. Keeping track on the last used world x position which been used to the calculation (for example .
    
    CCLayer *_gameLayer;    // The game's layer -> rendering the level, characters and anything else that happens inside the level
    CCSpriteBatchNode *_charactersBatchNode; // The characaters batch node
    b2World* _world;     // The Box2D physics world
    LevelHelperLoader* _loader;  // The level helper loading node, used for loading the level and query for tags (such as the start and finish lines)
    GameContactListener* _contactListener;  // The game's Box2D contact listener, used to test if the hero is touching the ground (of the level) or not. (If the hero is in the air it cannot jump)
    Hero * _player; // The Hero. Current player
    CCLabelBMFont * _scoreDisplay; // The bitmap found display for the game's score
    
    CCLayer *_hudLayer; // The hud layer -> show the "X" button, Jump button, "Use power up" button and the scores.
    CCSpriteBatchNode *_hudBatchNode;   // The HUD elements batch node
    CCSprite* _usePowerNode;    // Use the power up node (we switch the power up icon by the power up that the hero collected)
    CCMenu* _usePowerHudMenu; // The power up hud menu (we hide the power up menu in case the hero used the power up)
    
    PowerUp* _collectedPowerUp; // The power up that the hero collected, NULL if the hero didn't collect or already used
    CCArray *_powerUpBoxes; // Of type PowerUp. Will contain the random boxes that the player yet to pass (sorted by their X position on screen) (or been taken by other players)
    
    CCArray *_heroFireBalls; // Of type HeroFireBall. Will contain the current (active - moving) fire balls (lasers) that the hero fired to the level.
    
    void update (float dt); // The main update loop for the game
    
    void loadLevelAssets(); // Loading level assets for the game such as batch nodes and HUD elements
    
    float locateStartLine(); // Locating & return the start line in the current level based on the LevelLoader tag
    float locateFinishLine(); // Locating & return the finish line in the current level based on the LevelLoader tag
    void locateRandomBoxes(); // Locating the random boxes in the level based on the LevelLoader tag
    void loadHUDElements(); // Loading the HUD elements (score, "X" button, "Jump" button) to the hud layer.
    void dispatchFireBall(uint32_t fromPowerUpId); // Dispatch a fire ball (laser) from the hero to the level using a given power up id
    
    void updateScoreLabel(); // Refresh the score label with the score member value
    void showBonusLabel(int bonus); // Show the bonus effect on the screen (with animation effects)
    
    void preparePlayersForRace(float startLineScreenXPosition); // Arrange the players to the race, place them on the plateau until the start line
    void ccTouchesBegan(CCSet* pTouches, CCEvent* event); //  Begin touch event callback, used for the execute the jump event for the hero in case the user tap on the screen
    
    void callbackDoneEndRaceAnimation(); // When the race end we shows the bonus which the hero got. Once the animation finished we need to switch back to the main menu. The finish animation execute this callback after a delay
    void menuCallbackEndGame(CCObject* pSender); // Callback for the HUD menu's "X" button, will end the game and switch back to the main menu
    void menuCallbackJump(CCObject* pSender); // Callback for the HUD menu's "Jump" button, will trigger a jump for the hero
    void menuCallbackUsePower(CCObject* pSender); // Callback for the HUD menu's "Use Powerup" button, use the powerup if the hero has any
    
    static bool powerUpBoxScreenPositionNodeComparator(CCObject *box1, CCObject *box2); // Used to compare two random boxes by their screen position so the random boxes array will be filled by screen position (first random box is the first random box in the level)
public:
    ~GameLayer();
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static CCScene* scene();
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(GameLayer);
};

#endif /* defined(__UFORun__GameLayer__) */
