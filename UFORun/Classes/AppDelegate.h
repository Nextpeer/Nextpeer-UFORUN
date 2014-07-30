//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"
#include "NextpeerDelegate.h"

USING_NS_CC;

/**
 @brief    The cocos2d Application.
 
 The reason for implement as private inheritance is to hide some interface call by CCDirector.
 */
class  AppDelegate : private cocos2d::CCApplication
{
private:
    NextpeerDelegate _nextpeerDelegate;
    void initializeViewPort(CCSize viewSize, float scaleFactor);
    void initializeNextpeer();
    
public:
    AppDelegate();
    virtual ~AppDelegate();
    
    /**
     @brief    Implement CCDirector and CCScene init code here.
     @return true    Initialize success, app continue.
     @return false   Initialize failed, app terminate.
     */
    virtual bool applicationDidFinishLaunching();
    
    /**
     @brief  The function be called when the application enter background
     @param  the pointer of the application
     */
    virtual void applicationDidEnterBackground();
    
    /**
     @brief  The function be called when the application enter foreground
     @param  the pointer of the application
     */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

