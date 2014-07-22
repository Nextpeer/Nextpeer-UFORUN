
#include "LHContactListener.h"


LHContactListener::LHContactListener(){
}

LHContactListener::~LHContactListener() {
}

void LHContactListener::BeginContact(b2Contact* contact) {
    (*beginEndSolveSelector)( nodeObject, contact, true);
}

void LHContactListener::EndContact(b2Contact* contact) {
    (*beginEndSolveSelector)(nodeObject, contact, false);
}

void LHContactListener::PreSolve(b2Contact* contact, 
								 const b2Manifold* oldManifold) {
    (*preSolveSelector)( nodeObject, contact, oldManifold);
}

void LHContactListener::PostSolve(b2Contact* contact, 
								  const b2ContactImpulse* impulse) {
    (*postSolveSelector)(nodeObject, contact, impulse);
}