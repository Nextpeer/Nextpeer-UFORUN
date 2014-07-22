//
//  LHContactListener.h

#ifndef __LH_CONTACT_LISTERE__
#define __LH_CONTACT_LISTERE__
#include "Box2D/Box2D.h"
#include <vector>
#include <algorithm>

class LHContactListener : public b2ContactListener {
	
public:	
    void* nodeObject;
    
    void (*beginEndSolveSelector)(void*, 
                                  b2Contact* contact, bool isBegin);

    void (*preSolveSelector)(void*, 
                             b2Contact* contact, 
                             const b2Manifold* oldManifold);
    
    void (*postSolveSelector)(void*, 
                              b2Contact* contact, 
                              const b2ContactImpulse* impulse);
    
    LHContactListener();
    ~LHContactListener();
	
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);    
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	
};
#endif