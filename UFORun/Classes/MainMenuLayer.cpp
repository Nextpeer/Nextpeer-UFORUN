//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "MainMenuLayer.h"
#include "PlayerSpriteProfile.h"
#include "GameLayer.h"
#include "CCNextpeer.h"

using namespace nextpeer;
#define HUD_ITEMS_SPACING 10.0f

CCScene* MainMenuLayer::scene()
{
	CCScene * scene = NULL;
	do
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
        
		// 'layer' is an autorelease object
        MainMenuLayer *layer = MainMenuLayer::create();
		CC_BREAK_IF(! layer);
        
		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);
    
	// return the scene
	return scene;
}

MainMenuLayer::~MainMenuLayer()
{
    CC_SAFE_RELEASE_NULL(_availableAvatars);
}

// on "init" you need to initialize your instance
bool MainMenuLayer::init()
{
	bool bRet = false;
	do
	{
        //////////////////////////////
        // 1. super init first
        CC_BREAK_IF(! CCLayerGradient::initWithColor(ccc4(101, 212, 243, 255) , ccc4(96, 190, 218, 255), ccp(0,-1.0)));
        
        createScreen();
        
        bRet = true;
    } while (0);
    
    return bRet;
}

void MainMenuLayer::createScreen () {
    
    //get screen size
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    _selectedAvtarIndex = 0;
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Menu.plist");
    _menuBatchNode = CCSpriteBatchNode::create("Menu.png", 200);
    this->addChild(_menuBatchNode);
    
    // Add some stars to the back screen. plist is only supported on iOS so avoid that on other platforms.
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    MainMenuLayer::addChild(CCParticleSystemQuad::create("Stars1.plist"));
    MainMenuLayer::addChild(CCParticleSystemQuad::create("Stars2.plist"));
    MainMenuLayer::addChild(CCParticleSystemQuad::create("Stars2.plist"));
    MainMenuLayer::addChild(CCParticleSystemQuad::create("Stars3.plist"));
    MainMenuLayer::addChild(CCParticleSystemQuad::create("Stars2.plist"));
#endif
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Characters.plist");
    _charactersBatchNode = CCSpriteBatchNode::create("Characters.png", 200);
    this->addChild(_charactersBatchNode);
    
    // Place the logo on the top-center of the screen
    CCSprite *logo = CCSprite::createWithSpriteFrameName("menu_logo.png");
    logo->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.8));
    _menuBatchNode->addChild(logo);
    
    // Place the moon on the right-bottom corner of the screen
    CCSprite *moon = CCSprite::createWithSpriteFrameName("menu_moon.png");
    moon->setPosition(ccp(_screenSize.width - moon->getContentSize().width/2, moon->getContentSize().height/3));
    addChild(moon);
    
    // Place the earth on the left-bottom corner of the screen
    CCSprite *earth = CCSprite::createWithSpriteFrameName("menu_earth.png");
    earth->setPosition(ccp(earth->getContentSize().width/3, earth->getContentSize().height/4));
    addChild(earth);
    
    // Try to extract the current user profile avatar, if manage to place the avatar selection utils and animate the earth, moon out of the way.
    GamePlayerProfile *currentUserProfile = GamePlayerProfileFactory::createProfileByTypeOrNull(CurrentPlayerGameProfile::getCurrentUserProfile());
    if (currentUserProfile != NULL) {
        _availableAvatars = GamePlayerProfileFactory::allGamePlayerProfiles();
        _availableAvatars->retain();
        
        // Find the selected avatar index
        CCObject* it = NULL;
        _selectedAvtarIndex = 0;
        CCARRAY_FOREACH(_availableAvatars, it) {
            GamePlayerProfile *profile = static_cast<GamePlayerProfile*>(it);
            if (profile->profileType() == currentUserProfile->profileType()) {
                break;
            }
            _selectedAvtarIndex++;
        }

        _selectedAvatar = CCSprite::createWithSpriteFrameName(currentUserProfile->spriteFrontName());
        _selectedAvatar->setPosition(ccp(_screenSize.width*0.5, _selectedAvatar->getContentSize().height));
        _selectedAvatar->setOpacity(0);
        _charactersBatchNode->addChild(_selectedAvatar);
        
        // Select Avatar -> Right
        CCNode* avatarSelectionRightNode = CCSprite::createWithSpriteFrameName("menu_select_right.png");
        CCMenuItemSprite* avatarSelectionRightMenuItem = CCMenuItemSprite::create(avatarSelectionRightNode, avatarSelectionRightNode, this, menu_selector(MainMenuLayer::menuCallbackChangeAvatarToRight));
        _menuAvatarSelectionRight = CCMenu::create(avatarSelectionRightMenuItem, NULL);
        _menuAvatarSelectionRight->setPosition(ccp(_selectedAvatar->getPositionX() + avatarSelectionRightNode->getContentSize().width + HUD_ITEMS_SPACING, _selectedAvatar->getPositionY()));
        _menuAvatarSelectionRight->setOpacity(0);
        this->addChild(_menuAvatarSelectionRight);
        
        // Select Avatar -> Left
        CCNode* avatarSelectionLeftNode = CCSprite::createWithSpriteFrameName("menu_select_left.png");
        CCMenuItemSprite* avatarSelectionLeftMenuItem = CCMenuItemSprite::create(avatarSelectionLeftNode, avatarSelectionLeftNode, this, menu_selector(MainMenuLayer::menuCallbackChangeAvatarToLeft));
        _menuAvatarSelectionLeft = CCMenu::create(avatarSelectionLeftMenuItem, NULL);
        _menuAvatarSelectionLeft->setPosition(ccp(_selectedAvatar->getPositionX() - avatarSelectionLeftNode->getContentSize().width - HUD_ITEMS_SPACING, _selectedAvatar->getPositionY()));
        _menuAvatarSelectionLeft->setOpacity(0);
        this->addChild(_menuAvatarSelectionLeft);
        
        // Animation to hide the moon/earth and show the avatar selection controls
        earth->runAction(CCSpawn::create(CCMoveTo::create(1.5f, ccp(-earth->getContentSize().width, earth->getPositionY())), CCFadeOut::create(1.4f), NULL));
        moon->runAction(CCSpawn::create(CCMoveTo::create(1.5f, ccp(_screenSize.width + moon->getContentSize().width, moon->getPositionY())), CCFadeOut::create(1.4f), NULL));
        _selectedAvatar->runAction(CCSequence::create(CCDelayTime::create(0.1), CCFadeIn::create(0.4f), NULL));
        _menuAvatarSelectionRight->runAction(CCSequence::create(CCDelayTime::create(0.2), CCFadeIn::create(0.3f), NULL));
        _menuAvatarSelectionLeft->runAction(CCSequence::create(CCDelayTime::create(0.2), CCFadeIn::create(0.3f), NULL));
    }
    
    // Open Nextpeer
    CCNode* openNextpeerButtonNode = CCSprite::createWithSpriteFrameName("menu_play_normal.png");
    CCNode* openNextpeerSelectedButtonNode = CCSprite::createWithSpriteFrameName("menu_play_selected.png");
    CCMenuItemSprite* openNextpeerMenuItem = CCMenuItemSprite::create(openNextpeerButtonNode, openNextpeerSelectedButtonNode, this, menu_selector(MainMenuLayer::menuCallbackStartGame));
    CCMenu *openNextpeerMenu = CCMenu::create(openNextpeerMenuItem, NULL);
    openNextpeerMenu->setPosition(ccp(_screenSize.width*0.5, _screenSize.height*0.5));
    this->addChild(openNextpeerMenu);
}

void MainMenuLayer::menuCallbackStartGame(CCObject* pSender) {
    
    CCNextpeer::getInstance()->launchDashboard();
}

void MainMenuLayer::menuCallbackChangeAvatarToRight(CCObject* pSender) {
    if (_selectedAvtarIndex + 1 >= _availableAvatars->count()) {
        _selectedAvtarIndex = 0;
    }
    else {
        _selectedAvtarIndex++;
    }
    
    GamePlayerProfile *profile = (GamePlayerProfile *)_availableAvatars->objectAtIndex(_selectedAvtarIndex);
    
    CurrentPlayerGameProfile::setCurrentUserProfile(profile->profileType());
    _selectedAvatar->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(profile->spriteFrontName()));
}

void MainMenuLayer::menuCallbackChangeAvatarToLeft(CCObject* pSender) {
    if (_selectedAvtarIndex == 0) {
        _selectedAvtarIndex = _availableAvatars->count() - 1;
    }
    else {
        _selectedAvtarIndex--;
    }
    
    GamePlayerProfile *profile = (GamePlayerProfile *)_availableAvatars->objectAtIndex(_selectedAvtarIndex);
    
    CurrentPlayerGameProfile::setCurrentUserProfile(profile->profileType());
    _selectedAvatar->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(profile->spriteFrontName()));
}
