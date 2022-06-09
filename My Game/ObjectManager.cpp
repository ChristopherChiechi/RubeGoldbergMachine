/// \file ObjectManager.cpp
/// \brief Code for the object manager CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"
#include "Renderer.h"

#include "LineObject.h"

CObjectManager::CObjectManager(){
} //constructor

/// The destructor clears the object list, which destructs
/// all of the objects in it.

CObjectManager::~CObjectManager(){
  clear();
} //destructor

/// Delete all of the entities managed by object manager. 
/// This involves deleting all of the CObject instances pointed
/// to by the object list, then clearing the object list itself.

void CObjectManager::clear(){
  for(auto const& p: m_stdList) //for each object
    delete p; //delete object

  m_stdList.clear(); //clear the object list

  for (auto const& p : m_stdLineList) //for each line
      delete p; //delete line

  m_stdLineList.clear(); //clear the line list
} //clear

/// Draw the game objects using Painter's Algorithm.
/// The background is drawn first, then the game
/// objects are asked to draw themselves in the
/// order that they are in the object list. That is,
/// they are drawn from back to front.

void CObjectManager::draw(){  
  if(m_eDrawMode == eDrawMode::Sprites || m_eDrawMode == eDrawMode::Both)
    m_pRenderer->Draw(eSprite::Background, m_vWinCenter); //draw background

  for (auto const& p : m_stdLineList) //for each Pulleyline
      if (p != nullptr)
          p->draw();

  for(auto const& p: m_stdList) //for each object
      if (p != nullptr)
        p->draw(); //draw it in renderer

  
} //draw

/// Create world edges in Physics World.
/// Place Box2D edge shapes in the Physics World in places that correspond to the
/// bottom, right, and left edges of the screen in renderer. The left and
/// right edges continue upwards for a distance. There is no top to the world.

void CObjectManager::CreateWorldEdges(){
  const float w = RW2PW(m_nWinWidth); //window width in Physics World units
  const float h = RW2PW(m_nWinHeight); //window height in Physics World units

  //corners of the window
  const b2Vec2 vBLeft  = b2Vec2(0, 0); //bottom left
  const b2Vec2 vBRight = b2Vec2(w, 0); //bottom right
  const b2Vec2 vTLeft  = b2Vec2(0, h); //top left
  const b2Vec2 vTRight = b2Vec2(w, h); //top right

  //Box2D ground
  b2BodyDef bd; //body definition
  b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd); //body
  b2EdgeShape shape; //shape

  //bottom of window
  shape.m_vertex1 = vBLeft;
  shape.m_vertex2 = vBRight;
  pBody->CreateFixture(&shape, 0);

  //left edge of window
  shape.m_vertex1 = vBLeft;
  shape.m_vertex2 = vTLeft;
  pBody->CreateFixture(&shape, 0);

  //right edge of window
  shape.m_vertex1 = vBRight;
  shape.m_vertex2 = vTRight;
  pBody->CreateFixture(&shape, 0);
} //CreateWorldEdges

/// Create an object in object manager and link its Physics World
/// body to it.
/// \param t Sprite type.
/// \param p Pointer to Box2D body.

void CObjectManager::CreateObject(eSprite t, b2Body* p){
  CObject* pObj = new CObject(t, p);
  m_stdList.push_back(pObj);
  p->GetUserData().pointer = (uintptr_t)pObj;
} //CreateObject

CLineObject* CObjectManager::CreateLine(b2Body* b0, const b2Vec2& d0, bool r0,
    b2Body* b1, const b2Vec2& d1, bool r1)
{
    CLineObject* p = new CLineObject(b0, d0, r0, b1, d1, r1);
    m_stdLineList.push_back(p);
    return p;
} //CreateLine