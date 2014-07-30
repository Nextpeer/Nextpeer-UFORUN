//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#ifndef __UFORun__SpriteUpdateInterpolator__
#define __UFORun__SpriteUpdateInterpolator__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Box2D.h"

USING_NS_CC;
USING_NS_CC_EXT;


/**
 Without some sort of extrapolation algorithm, the sprites will only move by network updates snap between a position to position.
 Those updates can arrive with significant latency, or not arrive at all. Latency also makes player movement look choppy.
 One popular prediction algorithm is “Dead Reckoning” (DR).
 
 Dead Reckoning -
 Dead Reckoning is a technique for latency hiding and bandwidth reduction,
 which developed out of the Defense Advanced Research Project Agency's (DARPA) SIMNET project (under the Distributed Interactive Simulation protocol). 
 DR is the process of calculating one network entity’s current position by using a previously determined position, or fix, 
 and advancing that position based upon known or estimated speeds over elapsed time, and course (Source: http://en.wikipedia.org/wiki/Dead_reckoning#Dead_reckoning_for_networked_games).
 
 This class apply the Dead Reckoning extrapolation algorithm on the CCPhysicsSprite. Manipulates the object position/physics velocity to adapt the updates as they arrive from the network.
 The class will take into consideration the temporal delay (lag) between updates, as well as the current velocity of the sprite.
 One can update this class to use different sprite class then CCPhysicsSprite. For example if the project didn't use Box2D but simple animation. Then instead of using physics rules you could apply Movement animation.
 */
class SpriteUpdateInterpolator : public CCObject {
private:
    float _ignoreScreenDistanceDeltaThreshold;
    float _errorScreenDistanceDeltaThreshold;
    
    
    float *_temporalDelaysHistory;
    int _temporalDelaysHistoryIndex;
    
    CCPhysicsSprite *_sprite;
    
    float recalculateTemporalDelayBasedOnHistory(float temporalDelayFromNetwork);
public:
	
	SpriteUpdateInterpolator(CCPhysicsSprite *sprite, float ignoreScreenDistanceDeltaThreshold, float errorScreenDistanceDeltaThreshold);
    ~SpriteUpdateInterpolator();
    void updateSpritePositionFromNetworkUpdate(float temporalDelay, b2Vec2 worldUpdatePosition, b2Vec2 networkUpdateVelocity, bool isStuck);
};

#endif /* defined(__UFORun__SpriteUpdateInterpolator__) */
