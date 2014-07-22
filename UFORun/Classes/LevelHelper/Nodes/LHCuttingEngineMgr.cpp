//
//  LHCuttingEngineMgr.m
//  LevelHelperExplodingSprites
//
//  Created by Bogdan Vladu on 3/10/12.
//  Copyright (c) 2012 Bogdan Vladu. All rights reserved.
//

#include "LHCuttingEngineMgr.h"
#include "LHSprite.h"
#include "../LevelHelperLoader.h"

// Include STL vector class.
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

bool isLeft(b2Vec2 a, b2Vec2 b, b2Vec2 c);
int comparator(const void *a, const void *b);

// Typedef an STL vector of vertices which are used to represent
// a polygon/contour and a series of triangles.
typedef std::vector< b2Vec2 > Vector2dVector;

class Triangulate
{
public:

// triangulate a contour/polygon, places results in STL vector
// as series of triangles.
static bool Process(const Vector2dVector &contour,
                    Vector2dVector &result);

// compute area of a contour/polygon
static float Area(const Vector2dVector &contour);

// decide if point Px/Py is inside triangle defined by
// (Ax,Ay) (Bx,By) (Cx,Cy)
static bool InsideTriangle(float Ax, float Ay,
                           float Bx, float By,
                           float Cx, float Cy,
                           float Px, float Py);


private:
static bool Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V);

};


static const float EPSILON=0.0000000001f;

float Triangulate::Area(const Vector2dVector &contour)
{
    
    int n = contour.size();
    
    float A=0.0f;
    
    for(int p=n-1,q=0; q<n; p=q++)
    {
        A+= contour[p].x*contour[q].y - contour[q].x*contour[p].y;
    }
    return A*0.5f;
}

/*
 InsideTriangle decides if a point P is Inside of the triangle
 defined by A, B, C.
 */
bool Triangulate::InsideTriangle(float Ax, float Ay,
                                 float Bx, float By,
                                 float Cx, float Cy,
                                 float Px, float Py)

{
    float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    float cCROSSap, bCROSScp, aCROSSbp;
    
    ax = Cx - Bx;  ay = Cy - By;
    bx = Ax - Cx;  by = Ay - Cy;
    cx = Bx - Ax;  cy = By - Ay;
    apx= Px - Ax;  apy= Py - Ay;
    bpx= Px - Bx;  bpy= Py - By;
    cpx= Px - Cx;  cpy= Py - Cy;
    
    aCROSSbp = ax*bpy - ay*bpx;
    cCROSSap = cx*apy - cy*apx;
    bCROSScp = bx*cpy - by*cpx;
    
    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};

bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
{
    int p;
    float Ax, Ay, Bx, By, Cx, Cy, Px, Py;
    
    Ax = contour[V[u]].x;
    Ay = contour[V[u]].y;
    
    Bx = contour[V[v]].x;
    By = contour[V[v]].y;
    
    Cx = contour[V[w]].x;
    Cy = contour[V[w]].y;
    
    if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;
    
    for (p=0;p<n;p++)
    {
        if( (p == u) || (p == v) || (p == w) ) continue;
        Px = contour[V[p]].x;
        Py = contour[V[p]].y;
        if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
    }
    
    return true;
}

bool Triangulate::Process(const Vector2dVector &contour,Vector2dVector &result)
{
    /* allocate and initialize list of Vertices in polygon */
    
    int n = contour.size();
    if ( n < 3 ) return false;
    
    int *V = new int[n];
    
    /* we want a counter-clockwise polygon in V */
    
    if ( 0.0f < Area(contour) )
        for (int v=0; v<n; v++) V[v] = v;
    else
        for(int v=0; v<n; v++) V[v] = (n-1)-v;
    
    int nv = n;
    
    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int count = 2*nv;   /* error detection */
    
    for(int m=0, v=nv-1; nv>2; )
    {
        /* if we loop, it is probably a non-simple polygon */
        if (0 >= (count--))
        {
            //** Triangulate: ERROR - probable bad polygon!
            return false;
        }
        
        /* three consecutive vertices in current polygon, <u,v,w> */
        int u = v  ; if (nv <= u) u = 0;     /* previous */
        v = u+1; if (nv <= v) v = 0;     /* new v    */
        int w = v+1; if (nv <= w) w = 0;     /* next     */
        
        if ( Snip(contour,u,v,w,nv,V) )
        {
            int a,b,c,s,t;
            
            /* true names of the vertices */
            a = V[u]; b = V[v]; c = V[w];
            
            /* output Triangle */
            result.push_back( contour[a] );
            result.push_back( contour[b] );
            result.push_back( contour[c] );
            
            m++;
            
            /* remove v from remaining polygon */
            for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;
            
            /* resest error detection counter */
            count = 2*nv;
        }
    }
    
    delete V;
    
    return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//a is point 1 on the line - b is point 2 on the line
//c is the point we want to check
bool isLeft(b2Vec2 a, b2Vec2 b, b2Vec2 c){
    return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class AllBodiesRayCastCallback : public b2RayCastCallback{
public:
AllBodiesRayCastCallback(){    
}

float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                      const b2Vec2& normal, float32 fraction){

    LHSprite* userData = (LHSprite*)fixture->GetBody()->GetUserData();

    if(LHSprite::isLHSprite(userData)){
        rayCastInfo[fixture->GetBody()] = point;
    }
    
    return 1;//go to all other points
}
std::map<b2Body*, b2Vec2> rayCastInfo;
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class BodiesInAABBCallback : public b2QueryCallback
{
public:
	virtual ~BodiesInAABBCallback() {}
    
	/// Called for each fixture found in the query AABB.
	/// @return false to terminate the query.
	bool ReportFixture(b2Fixture* fixture)
    {
        queryInfo[fixture->GetBody()] = fixture;
        return true;
    }
    std::map<b2Body*, b2Fixture*> queryInfo;
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//this code is an extension from here http://www.cocos2d-iphone.org/forum/topic/2079
void LHCuttingEngineMgr::explodeSpritesInRadius(float radius,
                                                float maxForce,
                                                CCPoint pos,
                                                b2World* world,
                                                bool doSuction)
{
    BodiesInAABBCallback callback;
    b2AABB aabb;
    
    aabb.lowerBound = LevelHelperLoader::pointsToMeters(ccp(pos.x - radius, pos.y - radius));
    aabb.upperBound = LevelHelperLoader::pointsToMeters(ccp(pos.x + radius, pos.y + radius));

    world->QueryAABB(&callback, aabb);
    
    std::map<b2Body*, b2Fixture*>::iterator it;
    
    for(it = callback.queryInfo.begin(); it != callback.queryInfo.end(); ++it)
    {
        b2Body* b = (*it).first;    
    
		b2Vec2 b2TouchPosition = LevelHelperLoader::pointsToMeters(pos);
		b2Vec2 b2BodyPosition = b2Vec2(b->GetPosition().x, b->GetPosition().y);
        
		float maxDistance = radius/LevelHelperLoader::meterRatio();
		float distance = 0.0f;
		float strength = 0.0f;
		float force = 0.0f;
		float angle = 0.0f;
        
		if(doSuction) 
		{
			distance = b2Distance(b2BodyPosition, b2TouchPosition);
			if(distance > maxDistance) distance = maxDistance - 0.01;
			// Get the strength
			//strength = distance / maxDistance; // Uncomment and reverse these two. and ones further away will get more force instead of less
			strength = (maxDistance - distance) / maxDistance; // This makes it so that the closer something is - the stronger, instead of further
			force  = strength * maxForce;
            
			// Get the angle
			angle = atan2f(b2TouchPosition.y - b2BodyPosition.y, b2TouchPosition.x - b2BodyPosition.x);
            b->ApplyForce(b2Vec2(cosf(angle) * force, sinf(angle) * force), b->GetPosition());
		}
		else
		{
			distance = b2Distance(b2BodyPosition, b2TouchPosition);
			if(distance > maxDistance) distance = maxDistance - 0.01;
            
			strength = (maxDistance - distance) / maxDistance;
			force = strength * maxForce;
			angle = atan2f(b2BodyPosition.y - b2TouchPosition.y, b2BodyPosition.x - b2TouchPosition.x);
            b->ApplyForce(b2Vec2(cosf(angle) * force, sinf(angle) * force), b->GetPosition());
		}
	}
}


LHCuttingEngineMgr* LHCuttingEngineMgr::sharedInstance(){
	static LHCuttingEngineMgr* sharedInstance = NULL;
	if (sharedInstance == NULL){
		sharedInstance = new LHCuttingEngineMgr();
	}
    return sharedInstance;
}
//------------------------------------------------------------------------------
LHCuttingEngineMgr::~LHCuttingEngineMgr()
{
    delete spritesPreviouslyCut;
}
//------------------------------------------------------------------------------
LHCuttingEngineMgr::LHCuttingEngineMgr()
{
    
    
#if COCOS2D_VERSION >= 0x00020000
    spritesPreviouslyCut = new CCDictionary();
    
    mShaderProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture);
    mColorLocation = glGetUniformLocation( mShaderProgram->getProgram(), "u_color");
#else
    spritesPreviouslyCut = new CCMutableDictionary<std::string>();
#endif
}

void LHCuttingEngineMgr::destroyAllPrevioslyCutSprites(){
#if COCOS2D_VERSION >= 0x00020000

    CCArray* keys = spritesPreviouslyCut->allKeys();
    if(keys){
        for(int i = 0; i < keys->count(); ++i){
            LHSprite* spr = (LHSprite*)spritesPreviouslyCut->objectForKey(((CCString*)keys->objectAtIndex(i))->getCString());
            if(spr)
                spr->removeSelf();
        }
    }
    spritesPreviouslyCut->removeAllObjects();
    
#else
    
    std::vector<std::string> keys = spritesPreviouslyCut->allKeys();
    
    for(int i = 0; i< keys.size(); ++i)
    {
        LHSprite* spr = (LHSprite*)spritesPreviouslyCut->objectForKey(keys[i]);
        if(spr)
            spr->removeSelf();
    }
    spritesPreviouslyCut->removeAllObjects();
    
#endif
}
//------------------------------------------------------------------------------
CCArray* LHCuttingEngineMgr::getSprites(){


    
#if COCOS2D_VERSION >= 0x00020000
    CCArray* array = CCArray::create();
    CCArray* keys = spritesPreviouslyCut->allKeys();
    if(keys){
        for(int i = 0; i < keys->count(); ++i){
            LHSprite* spr = (LHSprite*)spritesPreviouslyCut->objectForKey(((CCString*)keys->objectAtIndex(i))->getCString());
            if(spr){
                array->addObject(spr);
            }
        }
    }
#else
    CCArray* array = CCArray::array();
    std::vector<std::string> keys = spritesPreviouslyCut->allKeys();
    for(size_t i = 0; i< keys.size(); ++i){
        LHSprite* spr = (LHSprite*)spritesPreviouslyCut->objectForKey(keys[i]);
        if(spr)
            array->addObject(spr);
    }
#endif
    
    spritesPreviouslyCut->removeAllObjects();
    return array;
}
//------------------------------------------------------------------------------
LHSprite * LHCuttingEngineMgr::spriteWithVertices(CCPoint* vertices,
                                                  int count,
                                                  LHSprite* oldSprite){

    if(oldSprite == NULL)
    {
        CCLog("OLD SPRITE WAS NIL");
        return NULL;
    }
    
    if(!LHSprite::isLHSprite(oldSprite))
    {
        CCLog("OLD SPRITE IS NOT LHSprite");
        return NULL;
    }

    CCRect oldRect = oldSprite->getOriginalRect();
        
    CCTexture2D* oldTexture = CCTextureCache::sharedTextureCache()->addImage(oldSprite->getImageFile().c_str());
    
#if COCOS2D_VERSION >= 0x00020000
    CCSprite* tempOrigSprite = CCSprite::createWithTexture(oldTexture, oldRect);
#else
    CCSprite* tempOrigSprite = CCSprite::spriteWithTexture(oldTexture, oldRect);
#endif
    
    tempOrigSprite->setFlipX(true);
    tempOrigSprite->setFlipY(true);
    
#if COCOS2D_VERSION >= 0x00020000
    CCRenderTexture *justSprTx = CCRenderTexture::create((int)oldRect.size.width, (int)oldRect.size.height);
#else
    CCRenderTexture *justSprTx = CCRenderTexture::renderTextureWithWidthAndHeight(oldRect.size.width, oldRect.size.height);
#endif
    
    justSprTx->beginWithClear(1, 1, 1, 0);
    tempOrigSprite->draw();
    justSprTx->end();

#if COCOS2D_VERSION >= 0x00020000
    CCRenderTexture *myCutTexture = CCRenderTexture::create(oldRect.size.width, oldRect.size.height);
#else
    CCRenderTexture *myCutTexture = CCRenderTexture::renderTextureWithWidthAndHeight(oldRect.size.width, oldRect.size.height);
#endif
    
    myCutTexture->beginWithClear(1, 1, 1, 0);
    

#if COCOS2D_VERSION >= 0x00020000

    mShaderProgram->use();
    
#if COCOS2D_VERSION >= 0x00020100
    mShaderProgram->setUniformsForBuiltins();
#else
    mShaderProgram->setUniformForModelViewProjectionMatrix();
#endif
    
	ccVertex2F* verts = new ccVertex2F[count];
	for( int i=0;i<count;i++) {
		verts[i].x = vertices[i].x;//*CC_CONTENT_SCALE_FACTOR();
		verts[i].y = vertices[i].y;//*CC_CONTENT_SCALE_FACTOR();
	}
    
    ccTex2F* uvs = new ccTex2F[count];
	for( int i=0;i<count;i++) {
		uvs[i].u = (vertices[i].x/(float)justSprTx->getSprite()->getTexture()->getPixelsWide())*CC_CONTENT_SCALE_FACTOR();
		uvs[i].v = (vertices[i].y/(float)justSprTx->getSprite()->getTexture()->getPixelsHigh())*CC_CONTENT_SCALE_FACTOR();
	}
    
    ccGLBindTexture2D( justSprTx->getSprite()->getTexture()->getName() );
    
	mShaderProgram->setUniformLocationWith4f(mColorLocation,1.0f, 1.0f, 1.0f, 1.0f);
    
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, uvs);
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, verts);
	glDrawArrays(GL_TRIANGLES, 0, count);
    
	CC_INCREMENT_GL_DRAWS(1);
    delete[] uvs;
    delete[] verts;
    
#else
    glEnable(GL_TEXTURE_2D);		
    glBindTexture(GL_TEXTURE_2D, justSprTx->getSprite()->getTexture()->getName());
    
    ccVertex2F* verts = new ccVertex2F[count];
	for( int i=0;i<count;++i) {
		verts[i].x = vertices[i].x*CC_CONTENT_SCALE_FACTOR();
		verts[i].y = vertices[i].y*CC_CONTENT_SCALE_FACTOR();
	}
    
    ccColor4F* clr = new ccColor4F[count];
    for(int i = 0; i<count; ++i)
    {
        clr[i] = (ccColor4F){1.0f, 1.0f, 1.0f, 1.0f};
    }
    
    ccTex2F* uv = new ccTex2F[count];
    for(int k = 0; k < count; ++k){
        
        uv[k].u = (vertices[k].x/(float)justSprTx->getSprite()->getTexture()->getPixelsWide())*CC_CONTENT_SCALE_FACTOR();
        uv[k].v = (vertices[k].y/(float)justSprTx->getSprite()->getTexture()->getPixelsHigh())*CC_CONTENT_SCALE_FACTOR();
    }
    
    glTexCoordPointer(2, GL_FLOAT, 0, uv);
    glColorPointer(4, GL_FLOAT, 0, clr);
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_TRIANGLES, 0, count);
    delete[] uv;
    delete[] clr;
    delete[] verts;
    
#endif

    
    myCutTexture->end();
    

    
    LHSprite* sprCut = LHSprite::spriteWithTexture(myCutTexture->getSprite()->getTexture() );
    
    if(sprCut)
    {
        sprCut->setOrinalRect(oldRect);
        sprCut->setImageFile(oldSprite->getImageFile());
        sprCut->setTag(oldSprite->getTag());
        sprCut->setOpacity(oldSprite->getOpacity());
        sprCut->setColor(oldSprite->getColor());
        
        sprCut->setScaleX(oldSprite->getScaleX());
        sprCut->setScaleY(oldSprite->getScaleY());
        sprCut->setUsePhysicsForTouches(true);
        
        static long long createdSprites = 0;
        sprCut->setUniqueName(stringFromInt(createdSprites));
        ++createdSprites;
        
        
#if COCOS2D_VERSION >= 0x00020000 
        if(oldSprite->getBatchNode())
#else
        if(oldSprite->isUsesBatchNode())
#endif
        {
            LHLayer* layer = (LHLayer*)oldSprite->getParent()->getParent();
            layer->addChild(sprCut);
        }
        else {
            LHLayer* layer = (LHLayer*)oldSprite->getParent();
            layer->addChild(sprCut);
        }
//        LevelHelperLoader::setTouchDispatcherForSpriteWithTag(sprCut, oldSprite->getTag());
        
        spritesPreviouslyCut->setObject(sprCut, sprCut->getUniqueName());
        spritesPreviouslyCut->removeObjectForKey(oldSprite->getUniqueName());
    }
    
    return sprCut;
}
//------------------------------------------------------------------------------
LHSprite* LHCuttingEngineMgr::createNewSpriteFromBodyInfo(b2Body* body, LHSprite* oldSprite)
{
    b2Fixture* fixture = body->GetFixtureList();

    std::vector<CCPoint>triangles;
    
    while (fixture) {
        
        b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
        
        Vector2dVector result;
        Vector2dVector polygon;
        
        for(int k = 0; k < poly->GetVertexCount(); ++k){
            polygon.push_back(poly->m_vertices[k]);                
        }
        
        Triangulate::Process(polygon, result);
        
        for(int i = 0; i < (int)result.size()/3; ++i)
        {
            CCPoint texPoint[3];
            
            texPoint[0] = LevelHelperLoader::metersToPoints(result[i*3+0]);
            texPoint[1] = LevelHelperLoader::metersToPoints(result[i*3+1]);
            texPoint[2] = LevelHelperLoader::metersToPoints(result[i*3+2]);
            
            texPoint[0].x /= oldSprite->getScaleX();
            texPoint[0].y /= oldSprite->getScaleY();
            
            texPoint[1].x /= oldSprite->getScaleX();
            texPoint[1].y /= oldSprite->getScaleY();
            
            texPoint[2].x /= oldSprite->getScaleX();
            texPoint[2].y /= oldSprite->getScaleY();
            
            
            texPoint[0] = ccp(oldSprite->getContentSize().width/2 - texPoint[0].x,
                              oldSprite->getContentSize().height/2 - texPoint[0].y);
            
            texPoint[1] = ccp(oldSprite->getContentSize().width/2 - texPoint[1].x,
                              oldSprite->getContentSize().height/2 - texPoint[1].y);
            
            texPoint[2] = ccp(oldSprite->getContentSize().width/2 - texPoint[2].x,
                              oldSprite->getContentSize().height/2 - texPoint[2].y);
            
            
            triangles.push_back(texPoint[0]);
            triangles.push_back(texPoint[1]);
            triangles.push_back(texPoint[2]);
        }
        
        fixture = fixture->GetNext();
    }   
    
    CCPoint* texPoints = new CCPoint[triangles.size()];
    
    for(int i = 0; i < (int)triangles.size(); ++i){                                                            
        texPoints[i] = triangles[i];
    }
    
    LHSprite* newSprite = spriteWithVertices(texPoints, triangles.size(), oldSprite);

    if(newSprite){
        newSprite->setFlipX(true);
    }  
    
    delete[] texPoints;
    
    return newSprite;
}
//------------------------------------------------------------------------------
b2Body* LHCuttingEngineMgr::createBodyWithPoints(b2Vec2* verts,
                                                 int count,
                                                 b2Body* oldBody,
                                                 b2Fixture* oldFixture)
{
    b2World* world = oldBody->GetWorld();
    
    if(world->IsLocked())
        CCLog("Box2d world is locked. Game will assert. Do not perform actions on a body when the Box2d world is locked. Trigger an action at the end of your tick method.");
    
    b2FixtureDef fixture;
    
    b2BodyDef bodyDef;	
    bodyDef.type = oldBody->GetType();        
    bodyDef.position = oldBody->GetPosition();
    bodyDef.angle = oldBody->GetAngle();
    b2Body* body = world->CreateBody(&bodyDef);
    
    bodyDef.fixedRotation = oldBody->IsFixedRotation();
    
    b2PolygonShape shape;
    
    shape.Set(verts, count);		
    
    fixture.density = oldFixture->GetDensity();
    fixture.friction =oldFixture->GetFriction();
    fixture.restitution = oldFixture->GetRestitution();
    fixture.filter = oldFixture->GetFilterData();
    
    fixture.isSensor = oldFixture->IsSensor();
    
    fixture.shape = &shape;
    body->CreateFixture(&fixture);
    
    //we use this define to figure out which version of Box2d the user has
    //its not nice that box2d does not have a compile time versioning just like cocos2d
#ifdef B2_EDGE_SHAPE_H 
    body->SetGravityScale(oldBody->GetGravityScale());
#endif
	body->SetSleepingAllowed(oldBody->IsSleepingAllowed());    
    body->SetBullet(oldBody->IsBullet());
    
    return body;
}
//------------------------------------------------------------------------------

LHSprite* LHCuttingEngineMgr::spriteWithVertices(b2Vec2* vertices,
                                                 int count,
                                                 LHSprite* oldSprite,
                                                 b2Body* splitBody,
                                                 b2Fixture* fixture,
                                                 float mass)
{    
    b2Body* newBody = createBodyWithPoints(vertices, count, splitBody, fixture);
    
    if(newBody->GetMass() < mass)
    {
       b2World* world =  newBody->GetWorld();
        
        world->DestroyBody(newBody);
        return NULL;
    }
    
    LHSprite* newSprite1 = createNewSpriteFromBodyInfo(newBody, oldSprite);
    
    if(newSprite1){
        newBody->SetUserData(newSprite1);
        newSprite1->setBody(newBody);
        newSprite1->setUsesOverloadedTransformations(false);
    }
    
    return newSprite1;
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::splitSprite(LHSprite* oldSprite, CCPoint location){
    splitSprite(oldSprite, location, false, 0);
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::splitSprite(LHSprite * oldSprite,
                                     CCPoint location,
                                     bool breakFixturesOutsidePoint,
                                     float mass)
{        
    b2Body* splitBody = oldSprite->getBody();

    if(splitBody == NULL)
        return;
    
    b2World* world = splitBody->GetWorld();

    b2Vec2 pointInBox2dCoord = LevelHelperLoader::pointsToMeters(location);
        
    b2Fixture* fixture = splitBody->GetFixtureList();
    
    while (fixture) {
        
        if(fixture->GetShape()->GetType() != b2Shape::e_polygon)
            return;
        
        b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
        
        if(fixture->TestPoint(pointInBox2dCoord))
        {
            b2Vec2 prevPoint = poly->GetVertex(0);
            
            for(int i = 1; i < poly->GetVertexCount(); ++i)
            {
                b2Vec2 point = poly->GetVertex(i);
                
                b2Vec2* vertices = new b2Vec2[3];
               
                vertices[0] = prevPoint;
                vertices[1] = point;
                vertices[2] = splitBody->GetLocalPoint(pointInBox2dCoord);
                
                spriteWithVertices(vertices,3, oldSprite, splitBody, fixture, mass);
                
                prevPoint = point;
                
                delete[] vertices;
            }
            
            b2Vec2* vertices = new b2Vec2[3];
            
            vertices[0] = poly->GetVertex(0);
            vertices[1] = poly->GetVertex(poly->GetVertexCount()-1);
            vertices[2] = splitBody->GetLocalPoint(pointInBox2dCoord);
                        
            
            spriteWithVertices(vertices, 3, oldSprite, splitBody, fixture, mass);

            delete[] vertices;
        }
        else {
                        
            Vector2dVector result;
            Vector2dVector polygon;
            
            for(int k = 0; k < poly->GetVertexCount(); ++k){
                polygon.push_back(poly->m_vertices[k]);                
            }
            
            Triangulate::Process(polygon, result);
            
            if(breakFixturesOutsidePoint)
            {
            for(size_t i = 0; i < result.size()/3; ++i)
            {
                b2Vec2* vertices = new b2Vec2[3];
                
                vertices[0] = result[i*3+0];
                vertices[1] = result[i*3+1];
                vertices[2] = result[i*3+2];
                      
                spriteWithVertices(vertices, 3, oldSprite, splitBody, fixture, mass);
                
                delete[] vertices;
            }
            }
            else {
                
                spriteWithVertices(poly->m_vertices, poly->GetVertexCount(), oldSprite, splitBody, fixture, mass);
            }
        }
        
        fixture = fixture->GetNext();
    }
    
    if(LHSprite::isLHSprite(oldSprite)){
        spritesPreviouslyCut->removeObjectForKey(oldSprite->getUniqueName());
        ((LHSprite*)oldSprite)->removeBodyFromWorld();//we force because of race condition
        ((LHSprite*)oldSprite)->removeSelf();
    }
    else{
        world->DestroyBody(splitBody);    
        oldSprite->removeFromParentAndCleanup(true);
    }
}
//------------------------------------------------------------------------------
bool LHCuttingEngineMgr::testCentroid(b2Vec2* vs, int count)
{
    if(count < 3)
        return false;
    
	b2Vec2 c; c.Set(0.0f, 0.0f);
	float32 area = 0.0f;
    
	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	b2Vec2 pRef(0.0f, 0.0f);
#if 0
	// This code would put the reference point inside the polygon.
	for (int32 i = 0; i < count; ++i)
	{
		pRef += vs[i];
	}
	pRef *= 1.0f / count;
#endif
    
	const float32 inv3 = 1.0f / 3.0f;
    
	for (int32 i = 0; i < count; ++i)
	{
		// Triangle vertices.
		b2Vec2 p1 = pRef;
		b2Vec2 p2 = vs[i];
		b2Vec2 p3 = i + 1 < count ? vs[i+1] : vs[0];
        
		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;
        
		float32 D = b2Cross(e1, e2);
        
		float32 triangleArea = 0.5f * D;
		area += triangleArea;
        
		// Area weighted centroid
		c += triangleArea * inv3 * (p1 + p2 + p3);
	}
    
	// Centroid
    if(area < b2_epsilon)
        return false;
    
    return true;
}
//------------------------------------------------------------------------------
int comparator(const void *a, const void *b) {
    const b2Vec2 *va = (const b2Vec2 *)a;
    const b2Vec2 *vb = (const b2Vec2 *)b;
    
    if (va->x > vb->x) {
        return 1;
    } else if (va->x < vb->x) {
        return -1;
    }
    return 0;
}

#define calculate_determinant_2x2(x1,y1,x2,y2) x1*y2-y1*x2
#define calculate_determinant_2x3(x1,y1,x2,y2,x3,y3) x1*y2+x2*y3+x3*y1-y1*x2-y2*x3-y3*x1
/*
 * Arranges all given points in a counter clockwise order
 */
b2Vec2* LHCuttingEngineMgr::makeVerticesCounterclockwise(b2Vec2* vertices, int count)
{
    float determinant;
    int iCounterClockWise = 1;
    int iClockWise = count - 1;
    int i;
    
    b2Vec2 referencePointA,referencePointB;
    b2Vec2 *sortedVertices = (b2Vec2*)calloc(count, sizeof(b2Vec2));
    
    // sort all vertices in ascending order according to their x-coordinate so we can get two points of a line
    qsort(vertices, count, sizeof(b2Vec2), comparator);
    
    sortedVertices[0] = vertices[0];
    referencePointA = vertices[0];          //leftmost point
    referencePointB = vertices[count-1];    //rightmost point
    
    // we arrange the points by filling our vertices in both clockwise and counter-clockwise directions using the determinant function
    for (i=1;i<count-1;i++)
    {
        determinant = calculate_determinant_2x3(referencePointA.x, referencePointA.y, referencePointB.x, referencePointB.y, vertices[i].x, vertices[i].y);
        if (determinant<0)
        {
            sortedVertices[iCounterClockWise++] = vertices[i];
        }
        else
        {
            sortedVertices[iClockWise--] = vertices[i];
        }//endif
    }//endif
    
    sortedVertices[iCounterClockWise] = vertices[count-1];
    return sortedVertices;
}

/*
 * Determines if a shape's vertices are acceptable by Box2D standards
 */
bool LHCuttingEngineMgr::areVerticesCompatibleWithBox2d(b2Vec2* vertices, int count)
{
    //check 1: polygons need to at least have 3 vertices
    if (count < 3){
        return false;
    }
    
    //check 2: the number of vertices cannot exceed b2_maxPolygonVertices
    if (count > b2_maxPolygonVertices){
        return false;
    }
    
    //check 3: Box2D needs the distance from each vertex to be greater than b2_epsilon
    int32 i;
    for (i=0; i<count; ++i){
        int32 i1 = i;
        int32 i2 = i + 1 < count ? i + 1 : 0;
        b2Vec2 edge = vertices[i2] - vertices[i1];
        if (edge.LengthSquared() <= b2_epsilon * b2_epsilon){
            return false;
        }
    }
    
    //check 4: Box2D needs the area of a polygon to be greater than b2_epsilon
    float32 area = 0.0f;
    
    b2Vec2 pRef(0.0f,0.0f);
    
    for (i=0; i<count; ++i){
        b2Vec2 p1 = pRef;
        b2Vec2 p2 = vertices[i];
        b2Vec2 p3 = i + 1 < count ? vertices[i+1] : vertices[0];
        
        b2Vec2 e1 = p2 - p1;
        b2Vec2 e2 = p3 - p1;
        
        float32 D = b2Cross(e1, e2);
        
        float32 triangleArea = 0.5f * D;
        area += triangleArea;
    }
    
    //we assign a value of 0.0001 since anything further is too small to see anyway
    if (area <= 0.0001){
        return true;
    }
    
    //check 5: Box2D requires that the shape be Convex.
    float determinant;
    float referenceDeterminant;
    b2Vec2 v1 = vertices[0] - vertices[count-1];
    b2Vec2 v2 = vertices[1] - vertices[0];
    referenceDeterminant = calculate_determinant_2x2(v1.x, v1.y, v2.x, v2.y);
    
    for (i=1; i<count-1; i++){
        v1 = v2;
        v2 = vertices[i+1] - vertices[i];
        determinant = calculate_determinant_2x2(v1.x, v1.y, v2.x, v2.y);
        //we use the determinant to check direction from one point to another. A convex shape's points should only go around in one direction. The sign of the determinant determines that direction. If the sign of the determinant changes mid-way, then we have a concave shape.
        if (referenceDeterminant * determinant < 0.0f){
            //if multiplying two determinants result to a negative value, we know that the sign of both numbers differ, hence it is concave
            return false;
        }
    }
    //check the last two vertices
    v1 = v2;
    v2 = vertices[0]-vertices[count-1];
    determinant = calculate_determinant_2x2(v1.x, v1.y, v2.x, v2.y);
    if (referenceDeterminant * determinant < 0.0f){
        return false;
    }
    //passed all tests
    return true;
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::createFixtureWithVertices(std::vector<b2Vec2>& fixtureVertices,
                                                   b2Body* body,
                                                   b2Fixture* fixture)
{
    int vsize = fixtureVertices.size();
    b2Vec2 *verts = new b2Vec2[vsize];
    
    for(size_t i = 0; i<fixtureVertices.size(); ++i){
        verts[i].x = fixtureVertices[i].x;
        verts[i].y = fixtureVertices[i].y;
    }
    
    b2Vec2 *sortedVertices = makeVerticesCounterclockwise(verts, vsize);
    if(areVerticesCompatibleWithBox2d(sortedVertices, vsize))
    {
       if(testCentroid(sortedVertices, vsize))
       {
           
        b2PolygonShape shape;
        shape.Set(sortedVertices, vsize);
        b2FixtureDef fixtureDef;
        
        fixtureDef.density = fixture->GetDensity();
        fixtureDef.friction =fixture->GetFriction();
        fixtureDef.restitution = fixture->GetRestitution();
        fixtureDef.filter = fixture->GetFilterData();
        fixtureDef.isSensor = fixture->IsSensor();
        
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
           
       }
       else{
           CCLOG("Centroid was NOT ok - dumped the fixture");   
       }
    }
    else {
        CCLOG("VERTICES NOT COMPATIBLE WITH BOX2d");
    }
    
    free(sortedVertices);
    delete[] verts;

}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::setInfoOnBody(b2Body* body, b2Body* splitBody)
{
    if(!body || !splitBody)
        return;
    //we use this define to figure out which version of Box2d the user has
    //its not nice that box2d does not have a compile time versioning just like cocos2d
#ifdef B2_EDGE_SHAPE_H 
    body->SetGravityScale(splitBody->GetGravityScale());
#endif
    body->SetSleepingAllowed(splitBody->IsSleepingAllowed());    
    body->SetBullet(splitBody->IsBullet());
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LHCuttingEngineMgr::splitBody(b2Body* splitBody, b2Vec2 origA, b2Vec2 origB, CCPoint A ,CCPoint B)
{
    b2Fixture* fixture = splitBody->GetFixtureList();
    
    LHSprite* oldSprite = (LHSprite*)splitBody->GetUserData();
    
    if(!LHSprite::isLHSprite(oldSprite))
        return;
    
    if(oldSprite->getImageFile() == "")
        return;
    
//    if((LHSprite*)oldSprite->isTouchedAtPoint(A))
//    {
//        CCLog("We dont't cut old sprite because A is inside");
//        //if point is inside the sprite we need to cancel touch or else we will have noise
//        return;
//    }
//
//    if((LHSprite*)oldSprite->isTouchedAtPoint(B))
//    {
//        CCLog("We don't cut old sprite because B is inside");
//        //if point is inside the sprite we need to cancel touch or else we will have noise
//        return;
//    }
    
    b2World* world = splitBody->GetWorld();
    
    b2Vec2 pointA= splitBody->GetLocalPoint(origA);
    b2Vec2 pointB= splitBody->GetLocalPoint(origB);
    
    b2RayCastInput input1;
    input1.p1 = LevelHelperLoader::pointsToMeters(A);
    input1.p2 = LevelHelperLoader::pointsToMeters(B);
    input1.maxFraction = 1.0f;
    
    b2RayCastInput input2;
    input2.p1 = LevelHelperLoader::pointsToMeters(B);
    input2.p2 = LevelHelperLoader::pointsToMeters(A);
    input2.maxFraction = 1.0f;
    
    b2BodyDef bodyDef;	
    bodyDef.type        = splitBody->GetType();
    bodyDef.position    = splitBody->GetPosition();
    bodyDef.angle       = splitBody->GetAngle();
    bodyDef.fixedRotation = splitBody->IsFixedRotation();
    
    b2Body* body1 = world->CreateBody(&bodyDef);
    b2Body* body2 = world->CreateBody(&bodyDef);
    
    while (fixture) {
        
        int32 childIndex = 0;        
        b2RayCastOutput output1;
        b2RayCastOutput output2;
        
        if(fixture->GetShape()->GetType() != b2Shape::e_polygon)
        {
            CCLOG("FIXTURE IS NOT POLYGON - CANCELING CUT");
            return;
        }
        
        b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
        
        //we use this define to figure out which version of Box2d the user has
        //its not nice that box2d does not have a compile time versioning just like cocos2d
#ifdef B2_EDGE_SHAPE_H 
        bool hit1 = poly->RayCast(&output1, input1,splitBody->GetTransform(), childIndex);
#else
        bool hit1 = poly->RayCast(&output1, input1, splitBody->GetTransform());
#endif
        b2Vec2 hitPoint1;
        
        if(hit1){
            hitPoint1 = input1.p1 + output1.fraction * (input1.p2 - input1.p1);            
        }
        //we use this define to figure out which version of Box2d the user has
        //its not nice that box2d does not have a compile time versioning just like cocos2d
#ifdef B2_EDGE_SHAPE_H 
        bool hit2 = poly->RayCast(&output2, input2,splitBody->GetTransform(), childIndex);
#else 
        bool hit2 = poly->RayCast(&output2, input2,splitBody->GetTransform());
#endif
        b2Vec2 hitPoint2;
        if(hit2){
            hitPoint2 = input2.p1 + output2.fraction * (input2.p2 - input2.p1);            
        }
        
        if(hit1 && hit2)
        {
            std::vector<b2Vec2>shape1Vertices;
            std::vector<b2Vec2>shape2Vertices;
            
            shape1Vertices.push_back(splitBody->GetLocalPoint(hitPoint1));            
            shape2Vertices.push_back(splitBody->GetLocalPoint(hitPoint1));
            
            //if we have 2 hits we can split the fixture - else we leave it as it is            
            for(int i = 0; i< poly->GetVertexCount(); ++i){                
                bool d = isLeft(pointA, pointB, poly->GetVertex(i));
                
                if(d){
                    shape1Vertices.push_back(poly->GetVertex(i));
                }
                else {
                    shape2Vertices.push_back(poly->GetVertex(i));
                }
			}
            
            if(shape1Vertices.size() < b2_maxPolygonVertices){
                shape1Vertices.push_back(splitBody->GetLocalPoint(hitPoint2));
            }
            
            if(shape2Vertices.size() < b2_maxPolygonVertices){
                shape2Vertices.push_back(splitBody->GetLocalPoint(hitPoint2));
            }
            
            if(shape1Vertices.size() >= 3 && shape1Vertices.size() <= b2_maxPolygonVertices)
            {
                createFixtureWithVertices(shape1Vertices, body1, fixture);
            }
            else {
               // NSLog(@"MORE POINTS IN SHAPE 1 %d", shape1Vertices.size());
            }
            
            if(shape2Vertices.size() >= 3 && shape2Vertices.size() <= b2_maxPolygonVertices)
            {
                createFixtureWithVertices(shape2Vertices, body2, fixture);
            }
            else {
              //  NSLog(@"MORE POINTS IN SHAPE 2 %d", shape2Vertices.size());
            }
            
        }
        else {
            //I JUST NEED TO CREATE THE FIXTURE AND PUT IT IN THE APPROPRIATE BODY

            std::vector<b2Vec2>shape1Vertices;
            std::vector<b2Vec2>shape2Vertices;
            
            b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
            
            for(int i = 0; i< poly->GetVertexCount(); ++i){
                bool d = isLeft(pointA, pointB, poly->GetVertex(i));
                
                if(d){
                    shape1Vertices.push_back(poly->GetVertex(i));
                }
                else {
                    shape2Vertices.push_back(poly->GetVertex(i));
                }
			}
            
            if(shape1Vertices.size() >= 3 && shape1Vertices.size() <= b2_maxPolygonVertices)
            {
                createFixtureWithVertices(shape1Vertices, body1, fixture);
            }
            else {
               // NSLog(@"MORE POINTS IN SHAPE 1b %d", shape1Vertices.size());
            }
            
            if(shape2Vertices.size() >= 3 && shape2Vertices.size() <= b2_maxPolygonVertices)
            {
                createFixtureWithVertices(shape2Vertices, body2, fixture);
            }
            else {
              //  NSLog(@"MORE POINTS IN SHAPE 2b %d", shape2Vertices.size());
            }
            
        }
        
        fixture = fixture->GetNext();
    }
    
    if(body1 != NULL)
    {
    if (body1->GetFixtureList() != NULL) //we have no fixture in this body - lets dump it
    {
        LHSprite* newSprite1 = createNewSpriteFromBodyInfo(body1, oldSprite);
    
        if(newSprite1){
            newSprite1->setUsesOverloadedTransformations(false);
            body1->SetUserData(newSprite1);
            newSprite1->setBody(body1);
        }
    }
    else {
        world->DestroyBody(body1);
        body1 = NULL;
    }
    }
    
    if(body2 != NULL)
    {
    if(body2->GetFixtureList() != NULL)
    {
        LHSprite* newSprite2 = createNewSpriteFromBodyInfo(body2, oldSprite);
    
        if(newSprite2){
            body2->SetUserData(newSprite2);
            newSprite2->setBody(body2);
            newSprite2->setUsesOverloadedTransformations(false);
        } 
    }
    else {
        world->DestroyBody(body2);
        body2 = NULL;
    }
    }
    
    if(body1)
        setInfoOnBody(body1, splitBody);
    if(body2)
        setInfoOnBody(body2, splitBody);
    
    if(LHSprite::isLHSprite(oldSprite)){
        
        spritesPreviouslyCut->removeObjectForKey(oldSprite->getUniqueName());
        ((LHSprite*)oldSprite)->removeBodyFromWorld();//we force because of race condition
        ((LHSprite*)oldSprite)->removeSelf();
    }
    else{
        world->DestroyBody(splitBody);    
        LevelHelperLoader::removeTouchDispatcherFromSprite(oldSprite);
        oldSprite->removeFromParentAndCleanup(true);
    }
        
    return;
}
//------------------------------------------------------------------------------
float LHCuttingEngineMgr::distanceBetweenPoints(b2Vec2 point1, b2Vec2 point2){
    float xd = point1.x - point2.x;
    float yd = point1.y - point2.y;
    return sqrtf(xd*xd + yd*yd);
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutFirstSpriteIntersectedByLine(CCPoint startPt,
                                                         CCPoint endPt,
                                                         b2World* world)
{
    b2Vec2 p1 = LevelHelperLoader::pointsToMeters(startPt);
	b2Vec2 p2 = LevelHelperLoader::pointsToMeters(endPt);
	b2Vec2 r = p2 - p1;
	if(r.LengthSquared() <= 0.0f){
        return;
    }
    
    AllBodiesRayCastCallback callback1;
    world->RayCast(&callback1, 
                   p1,
                   p2);
    
    AllBodiesRayCastCallback callback2;
    world->RayCast(&callback2, 
                   p2 ,
                   p1);
    
    float distance = 0.0f;
    b2Body* bodyToCut = NULL;
    b2Vec2 pointAOnBody;
    b2Vec2 pointBOnBody;
    
    std::map<b2Body*, b2Vec2>::iterator it;
    for(it = callback1.rayCastInfo.begin(); it != callback1.rayCastInfo.end(); ++it){
        b2Body* key = (*it).first;    
        std::map<b2Body*, b2Vec2>::iterator it2 = callback2.rayCastInfo.find(key);
        if(it2 != callback2.rayCastInfo.end()){
            float dist = LHCuttingEngineMgr::distanceBetweenPoints(key->GetPosition(), p1);
            
            if(bodyToCut == NULL){
                distance = dist;
                bodyToCut = key;
                pointAOnBody = (*it).second;
                pointBOnBody = (*it2).second;
            }
            else {
                
                if(dist < distance){
                    distance = dist;
                    bodyToCut = key;
                    pointAOnBody = (*it).second;
                    pointBOnBody = (*it2).second;
                }
            }            
        }
    }

    if(bodyToCut){
        splitBody(bodyToCut, pointAOnBody, pointBOnBody, startPt, endPt);
    }
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutFirstSpriteWithTag(int tag,
                                               CCPoint startPt,
                                               CCPoint endPt,
                                               b2World* world)
{
    b2Vec2 p1 = LevelHelperLoader::pointsToMeters(startPt);
	b2Vec2 p2 = LevelHelperLoader::pointsToMeters(endPt);
	b2Vec2 r = p2 - p1;
	if(r.LengthSquared() <= 0.0f){
        return;
    }

    AllBodiesRayCastCallback callback1;
    world->RayCast(&callback1, 
                   p1,
                   p2);
    
    AllBodiesRayCastCallback callback2;
    world->RayCast(&callback2, 
                   p2 ,
                   p1);
    
    float distance = 0.0f;
    b2Body* bodyToCut = NULL;
    b2Vec2 pointAOnBody;
    b2Vec2 pointBOnBody;
    
    std::map<b2Body*, b2Vec2>::iterator it;
    for(it = callback1.rayCastInfo.begin(); it != callback1.rayCastInfo.end(); ++it){
        b2Body* key = (*it).first;
        
        LHSprite* sprite = (LHSprite*)key->GetUserData();
        
        if(sprite && sprite->getTag() == tag){
            std::map<b2Body*, b2Vec2>::iterator it2 = callback2.rayCastInfo.find(key);
            if(it2 != callback2.rayCastInfo.end()){
                float dist = LHCuttingEngineMgr::distanceBetweenPoints(key->GetPosition(), p1);
                
                if(bodyToCut == NULL){
                    distance = dist;
                    bodyToCut = key;
                    pointAOnBody = (*it).second;
                    pointBOnBody = (*it2).second;
                }
                else {
                
                    if(dist < distance){
                        distance = dist;
                        bodyToCut = key;
                        pointAOnBody = (*it).second;
                        pointBOnBody = (*it2).second;
                    }
                }            
            }
        }
    }
    
    if(bodyToCut){
        LHSprite* sprite = (LHSprite*)bodyToCut->GetUserData();

        if (sprite && sprite->getTag() == tag) {
            splitBody(bodyToCut, pointAOnBody, pointBOnBody, startPt, endPt);
        }
    }
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutSprite(LHSprite* oldSprite, CCPoint startPt, CCPoint endPt)
{
    b2Vec2 p1 = LevelHelperLoader::pointsToMeters(startPt);
	b2Vec2 p2 = LevelHelperLoader::pointsToMeters(endPt);
	b2Vec2 r = p2 - p1;
	if(r.LengthSquared() <= 0.0f){
        return;
    }
    
    b2Body* oldBody = oldSprite->getBody();
    
    if(oldBody == NULL)
        return;
    
    b2World* world = oldBody->GetWorld();
    
    AllBodiesRayCastCallback callback1;
    world->RayCast(&callback1, 
                   p1 ,
                   p2);
    
    AllBodiesRayCastCallback callback2;
    world->RayCast(&callback2, 
                   p2 ,
                   p1);
    
    std::map<b2Body*, b2Vec2>::iterator it;
    for(it = callback1.rayCastInfo.begin(); it != callback1.rayCastInfo.end(); ++it){
        b2Body* key = (*it).first;    
        std::map<b2Body*, b2Vec2>::iterator it2 = callback2.rayCastInfo.find(key);
        if(it2 != callback2.rayCastInfo.end()){
            b2Vec2 pointA = (*it).second;
            b2Vec2 pointB = (*it2).second;
            
            LHSprite* sprite = (LHSprite*)key->GetUserData();

            if(oldSprite == sprite){
                splitBody(key, pointA, pointB, startPt, endPt);
            }
        }
    }
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutAllSpritesIntersectedByLine(CCPoint startPt,
                                                        CCPoint endPt,
                                                        b2World* world)

{
    b2Vec2 p1 = LevelHelperLoader::pointsToMeters(startPt);
	b2Vec2 p2 = LevelHelperLoader::pointsToMeters(endPt);
	b2Vec2 r = p2 - p1;
	if(r.LengthSquared() <= 0.0f){
        return;
    }
    
    AllBodiesRayCastCallback callback1;
    world->RayCast(&callback1, 
                   p1,
                   p2);
    
    AllBodiesRayCastCallback callback2;
    world->RayCast(&callback2, 
                   p2,
                   p1);
    
    std::map<b2Body*, b2Vec2>::iterator it;
    for(it = callback1.rayCastInfo.begin(); it != callback1.rayCastInfo.end(); ++it)
    {
        b2Body* key = (*it).first;    
        std::map<b2Body*, b2Vec2>::iterator it2 = callback2.rayCastInfo.find(key);
        if(it2 != callback2.rayCastInfo.end())
        {
            b2Vec2 pointA = (*it).second;
            b2Vec2 pointB = (*it2).second;
            
            splitBody(key, pointA, pointB, startPt, endPt);
        }
    }
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutAllSpritesWithTag(int tag,
                                              CCPoint startPt,
                                              CCPoint endPt,
                                              b2World* world){

    AllBodiesRayCastCallback callback1;
    world->RayCast(&callback1, 
                   LevelHelperLoader::pointsToMeters(startPt) ,
                   LevelHelperLoader::pointsToMeters(endPt));
    
    AllBodiesRayCastCallback callback2;
    world->RayCast(&callback2, 
                   LevelHelperLoader::pointsToMeters(endPt),
                   LevelHelperLoader::pointsToMeters(startPt));
    
    std::map<b2Body*, b2Vec2>::iterator it;
    for(it = callback1.rayCastInfo.begin(); it != callback1.rayCastInfo.end(); it++)
    {
        b2Body* key = (*it).first;    
        
        std::map<b2Body*, b2Vec2>::iterator it2 = callback2.rayCastInfo.find(key);
        if(it2 != callback2.rayCastInfo.end())
        {
            b2Vec2 pointA = (*it).second;
            b2Vec2 pointB = (*it2).second;
        
            LHSprite* sprite = (LHSprite*)key->GetUserData();

            if(sprite && LHSprite::isLHSprite(sprite) && sprite->getTag() == tag)
            {
                splitBody(key, pointA, pointB, startPt, endPt);
            }            
        }
    }
}
//------------------------------------------------------------------------------
float LHCuttingEngineMgr::randomFloatBetweenSmallAndBig(float smallNumber, float bigNumber) {
    float diff = bigNumber - smallNumber;

	srand ( time(NULL) );
    return (((float) (rand() % ((unsigned)RAND_MAX + 1)) / RAND_MAX) * diff) + smallNumber;
}
//------------------------------------------------------------------------------
void  LHCuttingEngineMgr::createExplosionWithCuts(int numberOfCuts,
                                                  float radius,
                                                  CCPoint explosionPoint)
{    
    explosionLines.clear();    
    for(int i = 0; i < numberOfCuts; ++i)
    {
        float cutAngle = randomFloatBetweenSmallAndBig(0, 360);
                
        float x = explosionPoint.x + radius * cos (cutAngle);
        float y = explosionPoint.y + radius * sin (cutAngle);
        
        float x1 = explosionPoint.x - radius * cos (cutAngle);
        float y1 = explosionPoint.y - radius * sin (cutAngle);
        
        explosionLines.push_back(CCPointMake(x, y));
        explosionLines.push_back(CCPointMake(x1, y1));
    }
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutSpritesFromPoint(CCPoint point,
                                             float radius,
                                             int numberOfCuts,
                                             b2World* world)
{
    createExplosionWithCuts(numberOfCuts, radius, point);
    
    for(size_t i = 0; i< explosionLines.size()/2; i +=2)
    {
        CCPoint lineA = explosionLines[i*2+0];
        CCPoint lineB = explosionLines[i*2+1];
        cutAllSpritesIntersectedByLine(lineA, lineB, world);
    }
}
//------------------------------------------------------------------------------
void LHCuttingEngineMgr::cutSpritesWithTag(int tag,
                                           CCPoint point,
                                           float radius,
                                           int numberOfCuts,
                                           b2World* world)
{
    createExplosionWithCuts(numberOfCuts, radius, point);
    
    for(size_t i = 0; i< explosionLines.size()/2; i +=2)
    {
        CCPoint lineA = explosionLines[i*2+0];
        CCPoint lineB = explosionLines[i*2+1];
        cutAllSpritesWithTag(tag, lineA, lineB, world);
    }
}

void LHCuttingEngineMgr::explodeSpritesInRadius(float radius,
                                                float maxForce,
                                                CCPoint pos,
                                                b2World* world)
{
    explodeSpritesInRadius(radius, maxForce, pos, world,false);
}

void LHCuttingEngineMgr::implodeSpritesInRadius(float radius,
                                                float maxForce,
                                                CCPoint pos,
                                                b2World* world)
{
    explodeSpritesInRadius(radius, maxForce, pos, world, true);
}

//------------------------------------------------------------------------------
void LHCuttingEngineMgr::debugDrawing(){
    
    for(size_t i = 0; i < explosionLines.size(); i+=2)
    {
#if COCOS2D_VERSION >= 0x00020000 
        
        //XXX - GLES 2.0 draw call here
#else
        
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);		
        glColor4f(1, 0, 0, 1);
        CCPoint vertices[2];
        
        vertices[0] = explosionLines[i];
        vertices[1] = explosionLines[i+1];
        
        glVertexPointer(2, GL_FLOAT, 0, &vertices);
        glDrawArrays(GL_LINES, 0, 2);
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPopMatrix();
#endif
    }
}
