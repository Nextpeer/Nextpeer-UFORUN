//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__MainMenuLayer__
#define __UFORun__MainMenuLayer__

#include "cocos2d.h"
USING_NS_CC;

/**
 The main menu layer, Start the game, select an avatar.
 */
class MainMenuLayer : public CCLayerGradient
{
public:
    ~MainMenuLayer();
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(MainMenuLayer);
    
private:
    CCSize _screenSize;
    CCSpriteBatchNode * _menuBatchNode;
    CCSpriteBatchNode *_charactersBatchNode;
    CCSprite *_selectedAvatar;
    CCMenu *_menuAvatarSelectionRight;
    CCMenu *_menuAvatarSelectionLeft;
    CCArray *_availableAvatars;
    unsigned int _selectedAvtarIndex;
    void createScreen();
    void menuCallbackStartGame(CCObject* pSender);
    void menuCallbackChangeAvatarToRight(CCObject* pSender);
    void menuCallbackChangeAvatarToLeft(CCObject* pSender);
};


#endif /* defined(__UFORun__MainMenuLayer__) */
