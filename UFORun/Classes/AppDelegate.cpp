//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "AppDelegate.h"
#include "ViewPort.h"
#include "MainMenuLayer.h"

#include "CCNextpeer.h"
using namespace nextpeer;

typedef struct tagResource
{
    cocos2d::CCSize size;
    char directory[100];
}Resource;

static Resource smallResource  =  { cocos2d::CCSizeMake(480, 320),   "iphone" };
static Resource mediumResource =  { cocos2d::CCSizeMake(1024, 768),  "ipad"   };
static Resource largeResource  =  { cocos2d::CCSizeMake(2048, 1536), "ipadhd" };
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(480, 320);

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
    
    pDirector->setOpenGLView(pEGLView);
	
	// Set the design resolution
    pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionExactFit);
    CCSize frameSize = pEGLView->getFrameSize();
    std::vector<std::string> searchPath;
    
    // In this demo, we select resource according to the frame's height.
    // If the resource size is different from design resolution size, you need to set contentScaleFactor.
    // We use the ratio of resource's height to the height of design resolution,
    // this can make sure that the resource's height could fit for the height of design resolution.
    
    // if the frame's height is larger than the height of medium resource size, select large resource.
    float scaleFactor = 1.0f;
    if (frameSize.height > mediumResource.size.height) {
        searchPath.push_back(largeResource.directory);
        scaleFactor = 4.0f;
    }
    // if the frame's height is larger than the height of small resource size, select medium resource.
    else if (frameSize.height > smallResource.size.height) {
        searchPath.push_back(mediumResource.directory);
        scaleFactor = 2.0f;
    }
    // if the frame's height is smaller than the height of medium resource size, select small resource.
    else {
        searchPath.push_back(smallResource.directory);
        scaleFactor = 1.0f;
    }
    pDirector->setContentScaleFactor(scaleFactor);
    
    CCFileUtils::sharedFileUtils()->setSearchPaths(searchPath);
    
    // Init the view port with the frame size and scale factor
    this->initializeViewPort(frameSize, scaleFactor);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);
    
    // Initialize Nextpeer
    this->initializeNextpeer();
    
    // run
    pDirector->runWithScene(MainMenuLayer::scene());
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void AppDelegate::initializeViewPort(CCSize viewSize, float scaleFactor)
{
    // Divide by the scale factor so that the view port remains the same for retina devices (converts to points)
    viewSize.width /= scaleFactor;
    viewSize.height /= scaleFactor;
    
    ViewPort* viewPort = ViewPort::getInstance();
    viewPort->initialize(viewSize);
}

void AppDelegate::initializeNextpeer()
{
    // 1) Change the project's bundle identifier from "com.nextpeer.uforun" to your own domain (for example "com.mycompany.uforun") in the project's info.plist & AndroidManifest.xml.
    // 2) Navigate to https://developers.nextpeer.com and create your iOS and an Android game.
    // 3) In the dashboard, add a Game Pool and connect both games you've created in step #2. That's how the users from each platform will be able to play with each other.
    // 4) Paste the iOS game key and the Android game key from our dashboard below.

    
    #error MAKE SURE TO DO THE STEPS ABOVE AND THEN REMOVE THIS LINE.
    // ** Pay attention - You will need to play a few sessions until Nextpeer recording feature will kick in ** /
    
    // Use the proper Nextpeer's game key according to the current platform.
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    const char* gameKey = "?"; // TODO: Place your iOS game key which you got from the dashboard
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    const char* gameKey = "?"; // TODO: Place your Android game key which you got from the dashboard
#endif
    
    CCNextpeer::getInstance()->initialize(gameKey);
    
    // Register for Nextpeer's events
    _nextpeerDelegate.registerForEvents();
}
