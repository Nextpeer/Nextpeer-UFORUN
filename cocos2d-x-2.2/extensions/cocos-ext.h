#ifndef __COCOS2D_EXT_H__
#define __COCOS2D_EXT_H__

#include "ExtensionMacros.h"

// Physics integration
#if CC_ENABLE_CHIPMUNK_INTEGRATION || CC_ENABLE_BOX2D_INTEGRATION
#include "physics_nodes/CCPhysicsDebugNode.h"
#include "physics_nodes/CCPhysicsSprite.h"
#endif

#endif /* __COCOS2D_EXT_H__ */
