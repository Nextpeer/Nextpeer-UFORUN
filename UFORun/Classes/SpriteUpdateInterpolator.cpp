//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "SpriteUpdateInterpolator.h"
#include "ViewPort.h"

#define MAX_TEMPORAL_DELAYS_HISTORY_SIZE 7
#define TEMPORAL_DELAY_UPPER_THRESHOLD 1.5f

SpriteUpdateInterpolator::~SpriteUpdateInterpolator(){
	
    CC_SAFE_RELEASE_NULL(_sprite);
    CC_SAFE_DELETE(_temporalDelaysHistory);
}


SpriteUpdateInterpolator::SpriteUpdateInterpolator(CCPhysicsSprite *sprite, float ignoreScreenDistanceDeltaThreshold, float errorScreenDistanceDeltaThreshold) {
    _sprite = sprite;
    _sprite->retain();
    
    _ignoreScreenDistanceDeltaThreshold = ignoreScreenDistanceDeltaThreshold;
    _errorScreenDistanceDeltaThreshold = errorScreenDistanceDeltaThreshold;
    _temporalDelaysHistoryIndex = -1;
    _temporalDelaysHistory =  new float[MAX_TEMPORAL_DELAYS_HISTORY_SIZE]();
}

// In case the network is really slow, we need to drop the temporal delay and draw the sprite in the past/future. Otherwise there wil be invalid prediction.
// To calculate if the temporal delay is slow, we save the last MAX_TEMPORAL_DELAYS_HISTORY_SIZE and in case their average is grather than TEMPORAL_DELAY_UPPER_THRESHOLD
// We will drop the temporal delay to zero.
float SpriteUpdateInterpolator::recalculateTemporalDelayBasedOnHistory(float temporalDelayFromNetwork) {
    
    if (_temporalDelaysHistoryIndex == -1) {
        _temporalDelaysHistoryIndex = 0;
    }
    else {
        _temporalDelaysHistoryIndex = (_temporalDelaysHistoryIndex + 1) % MAX_TEMPORAL_DELAYS_HISTORY_SIZE;
    }
    _temporalDelaysHistory[_temporalDelaysHistoryIndex] = temporalDelayFromNetwork;
    
    float temporalDelayAverage = 0;
    for(int index = 0; index < MAX_TEMPORAL_DELAYS_HISTORY_SIZE; index++) {
        temporalDelayAverage += _temporalDelaysHistory[_temporalDelaysHistoryIndex];
    }
    temporalDelayAverage = (temporalDelayAverage / MAX_TEMPORAL_DELAYS_HISTORY_SIZE);
    
    // Don't let the temporal delay to pass the threadshold
    if (abs(temporalDelayAverage) > TEMPORAL_DELAY_UPPER_THRESHOLD) {
        temporalDelayFromNetwork = TEMPORAL_DELAY_UPPER_THRESHOLD;
    }
    
    return temporalDelayFromNetwork;
}

void SpriteUpdateInterpolator::updateSpritePositionFromNetworkUpdate(float temporalDelay, b2Vec2 worldUpdatePosition, b2Vec2 networkUpdateVelocity, bool isStuck) {
    
    if (_sprite == NULL) return;
    
    temporalDelay = recalculateTemporalDelayBasedOnHistory(temporalDelay);
    
    ViewPort *viewPort = ViewPort::getInstance();
    
    // Calculate the velocity as it offset by the temporal delay. Drop the Y axe (This will save us from calculating the acceleration and gravity changes).
    b2Vec2 velocityOffsetByTemporalDelay = networkUpdateVelocity;
    velocityOffsetByTemporalDelay *= temporalDelay;
    velocityOffsetByTemporalDelay.y = 0; // Do not take into account the velocity on the Y axe
    
    b2Vec2 estimatedWorldPositionPostTemporalDelay = velocityOffsetByTemporalDelay + worldUpdatePosition;
    CCPoint estimatedScreenPositionPostTemporalDelay = viewPort->worldToScreenCoordinate(estimatedWorldPositionPostTemporalDelay);
    
    CCPoint spriteScreenPosition = _sprite->getPosition();
    b2Body* spriteBody = _sprite->getB2Body();
    
    // If this opponent is off screen, we can just update the position quickly
    if (!viewPort->isScreenElementOnScreen(estimatedScreenPositionPostTemporalDelay, _sprite->getContentSize())) {
        _sprite->setPosition(estimatedScreenPositionPostTemporalDelay);
        spriteBody->SetLinearVelocity(networkUpdateVelocity);
        return;
    }
    
    // Optimization (if the sprite is stuck then made sure we're stuck as well)
    if (isStuck) {
        
        CCPoint lastKnownScreenPosition = viewPort->worldToScreenCoordinate(worldUpdatePosition);
        
        // Snap into position if this position update is simply too far away
        float screenDistance = abs(lastKnownScreenPosition.getDistance(spriteScreenPosition));
        
        // If the distance is bigger than the ignore distance, just apply the position (no convergence algorithm will run on it). This may happen if the opponent got stuck in an obstacle.
        if (screenDistance > _ignoreScreenDistanceDeltaThreshold) {
            _sprite->setPosition(lastKnownScreenPosition);
        }
        
        // Optimization. The opponent is stuck, remove the linear velocity (this will generate better look and feel).
        spriteBody->SetLinearVelocity(b2Vec2(0, 0));
    }
    // Else, the opponent is running in the level. Set the next position.
    else {
        // Snap into position if this position update is simply too far away
        float screenDistance = abs(estimatedScreenPositionPostTemporalDelay.getDistance(spriteScreenPosition));
        if (screenDistance > _errorScreenDistanceDeltaThreshold) {
            
            _sprite->setPosition(estimatedScreenPositionPostTemporalDelay);
            
            // Velocity remains the same
            spriteBody->SetLinearVelocity(networkUpdateVelocity);
        }
        // If the distance between the estimate and the update is negligible, we'll skip the position update and merely update the velocity
        else if (screenDistance < _ignoreScreenDistanceDeltaThreshold) {
            // Interpoloated = positional + temporalDelay * updated velocity + updated velocity
            b2Vec2 interpolatedVelocity = worldUpdatePosition - spriteBody->GetPosition();
            networkUpdateVelocity *= 1.0f + temporalDelay;
            interpolatedVelocity += networkUpdateVelocity;
            
            spriteBody->SetLinearVelocity(interpolatedVelocity);
        }
        // Else, the distance difference cannot be ignored, but it is smaller than the error distance so there will be no position snapping.
        else {
            
            b2Vec2 currentPosition = viewPort->screenToWorldCoordinate(spriteScreenPosition);
            b2Vec2 positionalVector = (worldUpdatePosition - currentPosition);
            b2Vec2 interpolatedVelocity = positionalVector + networkUpdateVelocity;
            
            // Move the body by the updated velocity a bit to compensate for the delay.
            // This gives us an overall "lean" towards the velocity from the network update
            interpolatedVelocity += velocityOffsetByTemporalDelay;
            
            spriteBody->SetLinearVelocity(interpolatedVelocity);
        }
    }

}
