/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "GameDefines.h"
#include "Renderer.h"
#include "ComponentIncludes.h"

#include "Pulley.h"
#include "LineObject.h"
#include "Catapult.h"
#include "Bird.h"

/// Call renderer's Release function to do the required
/// Direct3D cleanup, then delete renderer and object manager.
/// Also delete Physics World, which MUST be deleted after
/// object manager because of way things are set up.

CGame::~CGame(){
  delete m_pObjectManager;
  delete m_pPhysicsWorld;
  delete m_pParticleEngine;

  delete m_pPulley;
  delete m_pCatapult;
  delete m_pBird;
} //destructor

/// Initialize renderer and object manager, load 
/// images and sounds, start the timer, and begin the game.

void CGame::Initialize(){
  m_pRenderer = new CRenderer; 
  m_pRenderer->Initialize(eSprite::Size);
  m_pRenderer->LoadImages(); //load images from xml file list

  //set up object manager and Physics World
  m_pObjectManager = new CObjectManager; //set up object manager
  m_pPhysicsWorld = new b2World(RW2PW(0, -1000)); //set up Physics World with gravity
  m_pObjectManager->CreateWorldEdges(); //create world edges at edges of window
  m_pPhysicsWorld->SetContactListener(&m_cContactListener); //load up my contact listener

  LoadSounds(); //load the sounds for this game
  
  m_pParticleEngine = new LParticleEngine2D(m_pRenderer);

  //now start the game
  BeginGame();
} //Initialize

/// Initialize the audio player and load game sounds.

void CGame::LoadSounds(){
  m_pAudio->Initialize(eSound::Size);
  
  m_pAudio->Load(eSound::Whoosh, "whoosh");
  m_pAudio->Load(eSound::Yay, "yay");
  m_pAudio->Load(eSound::Bonk, "bonk");
  m_pAudio->Load(eSound::Buzz, "buzz");
  m_pAudio->Load(eSound::Restart, "restart");
} //LoadSounds

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release(){
  delete m_pRenderer;
  m_pRenderer = nullptr; //for safety
} //Release

/// Clear any old objects out of object manager and get ready
/// to start again.

void CGame::BeginGame(){  
  m_pParticleEngine->clear();
  m_pAudio->stop();

  m_pObjectManager->clear(); //clear old objects

  CreateLevel();
} //BeginGame

/// Create the button (which is a pig) that signals the end of the Rube Goldberg machine.
/// \param x X coordinate of button in Physics World units.
/// \param y Y coordinate of button in Physics World units.

void CGame::CreateButton(float x, float y){
  //Physics World
  b2BodyDef bd; 
	bd.type = b2_staticBody;
  bd.position.Set(x, y);

  //shape
  b2PolygonShape s;
  float w, h; //width and height of sprite
  m_pRenderer->GetSize(eSprite::Pig, w, h);
  s.SetAsBox(RW2PW(w)/2.0f, RW2PW(h)/2.0f);

  //fixture
  b2FixtureDef fd;
	fd.shape = &s;
	fd.density = 1.0f;
	fd.restitution = 0.2f;

  //body
  b2Body* p = m_pPhysicsWorld->CreateBody(&bd);
  p->CreateFixture(&fd);

  //object manager
  m_pObjectManager->CreateObject(eSprite::Pig, p);
} //CreateButton

/// Place a ball in Physics World and object manager.
/// \param x Horizontal coordinate in Physics World units.
/// \param y Vertical coordinate in Physics World units.
/// \param xv Horizontal component of velocity.
/// \param yv Vectical component of velocity.

void CGame::CreateBall(float x, float y, float xv, float yv){ 
  //Physics World
  b2BodyDef bd;
	bd.type = b2_dynamicBody;
  bd.position.Set(x, y);
  //bd.linearVelocity.Set(xv, yv);

  //shape
  b2CircleShape s;
	s.m_radius = RW2PW(m_pRenderer->GetWidth(eSprite::Ball))/2.0f;

  //fixture
	b2FixtureDef fd;
	fd.shape = &s;
	//fd.density = 1.0f;
    fd.density = 1.0f;
	//fd.restitution = 0.8f;
    fd.restitution = 0.3f;

  //body
  b2Body* p = m_pPhysicsWorld->CreateBody(&bd);
  p->CreateFixture(&fd);

  //object manager
  m_pObjectManager->CreateObject(eSprite::Ball, p);
} //CreateBall

/// Poll the keyboard state and respond to the
/// key presses that happened since the last frame.

void CGame::KeyboardHandler(){
  m_pKeyboard->GetState(); //get current keyboard state 

  if (m_pKeyboard->TriggerDown(VK_F1)) // reset game
  {
      BeginGame();
      m_eGameState = eGameState::Initial;
  }

  if(m_pKeyboard->TriggerDown(VK_F2)){ //change draw mode
    m_eDrawMode = eDrawMode((UINT)m_eDrawMode + 1);
    if(m_eDrawMode == eDrawMode::Size)m_eDrawMode = eDrawMode(0);
  } //if

  if(m_pKeyboard->TriggerDown(VK_SPACE)){
    switch(m_eGameState){
      case eGameState::Initial:
          CreateBall(RW2PW(m_nWinWidth - 35), RW2PW(m_nWinHeight), -10.0f, 0.0f);
        m_eGameState = eGameState::Running;
        m_pAudio->play(eSound::Whoosh);
        m_fStartTime = m_pTimer->GetTime();
      break;

      case eGameState::Finished:
        BeginGame(); //begin again
        m_eGameState = eGameState::Initial;
        m_pAudio->play(eSound::Restart); //must be after BeginGame(), which stops all sounds
      break;

      default: m_pAudio->play(eSound::Buzz);
    } //switch
  } //if
} //KeyboardHandler

/// Ask object manager to draw the game objects. RenderWorld
/// is notified of the start and end of the frame so
/// that it can let Direct3D do its pipelining jiggery-pokery.

void CGame::RenderFrame(){ 
  m_pRenderer->BeginFrame();
    m_pObjectManager->draw(); //draw the objects
    m_pParticleEngine->Draw(); //draw particles
    DrawClock(); //draw the timer
    if(m_eGameState == eGameState::Initial)
      m_pRenderer->DrawCenteredText("Hit space to begin.");
    else if(m_eGameState == eGameState::Finished)
      m_pRenderer->DrawCenteredText("Hit space to reset.");
  m_pRenderer->EndFrame();
} //RenderFrame

/// Draw a digital clock at the top right of the window, even if the
/// camera is panned left or right.

void CGame::DrawClock(){ 
    const float dx = 68.0f; //distance from right of screen
    const float dy = 48.0f; //distance from top of screen
    const float x = m_pRenderer->GetCameraPos().x; //ensure we are in screen space

    const Vector2 pos = Vector2(dx, m_nWinHeight - dy);
    m_pRenderer->Draw(eSprite::ClockFace, pos); //clock background

    float t = 0.0f; //for the time

    switch (m_eGameState) { //set t depending on game state
    case eGameState::Initial:  t = 0.0f; break; //clock reads zero
    case eGameState::Running:  t = m_pTimer->GetTime() - m_fStartTime; break;
    case eGameState::Finished: t = m_fTotalTime; break; //clock is stopped
    default: t = 0.0f;
    } //switch

    const UINT min = (UINT)floorf(t / 60.0f); //minutes
    const UINT sec = (UINT)floorf(t - 60.0f * min); //seconds

    const std::string str = std::to_string(min) + ":" + //string to be drawn
        std::to_string(sec / 10) + std::to_string(sec % 10);

    const Vector2 pos2 = Vector2(dx - 45.0f, 18.0f); //text position
    m_pRenderer->DrawScreenText(str.c_str(), pos2, Colors::White); //draw in white
} //DrawClock

/// Handle keyboard input, move the game objects and render 
/// them in their new positions and orientations. Notify 
/// the timer of the start and end of the
/// frame so that it can calculate frame time. 

void CGame::ProcessFrame(){
  KeyboardHandler(); //handle keyboard input
  m_pAudio->BeginFrame(); //notify sound manager that frame has begun

  m_pTimer->Tick([&](){ 
    m_pPhysicsWorld->Step(m_pTimer->GetFrameTime(), 6, 2); //move all objects 
    m_pParticleEngine->step(); //move particles in particle effects
  });

  // move pulley
  if (m_pPulley)
      m_pPulley->move();

  // move catapult
  if (m_pCatapult->GetCollision())
      m_pCatapult->move();

  RenderFrame(); //render a frame of animation 
} //ProcessFrame

// set the vertices of polygons
b2Vec2 CGame::setVertice(float x, float y, eSprite e)
{
    b2Vec2 vertice; //vertice
    float w, h; //width and height of sprite
    m_pRenderer->GetSize(e, w, h);

    // Calculate the x and y coordinates.
    // Artist world coordinates are relative to the top-left corner of the sprite.
    // Physics world coordinates are relative to the center of the sprite.
    // x coordinate = subtract half of length (physics world center x coordinate), divide by 10
    // y coordinate = flip y, add add half of height (physics world center y coordinate), divide by 10.
    // y must be flipped because the origin in game is at the bottom-left corner, not the top-left corner.
    vertice.Set(RW2PW(x - w / 2.0f), RW2PW(-y + h / 2.0f));

    return vertice;
}

// create platform
void CGame::CreatePlatform(float x, float y, float a) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(RW2PW(x), RW2PW(y));
    bd.angle = a;

    // shape
    float w, h;
    m_pRenderer->GetSize(eSprite::Platform, w, h);
    b2PolygonShape s;
    s.SetAsBox(RW2PW(w) / 2.0f, RW2PW(h) / 2.0f);

    // fixture definition
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 10.0f;
    fd.restitution = 0.1f;

    // create body and fixture
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd);
    m_pObjectManager->CreateObject(eSprite::Platform, pBody);
}

// create small platform
void CGame::CreateSmallPlatform(float x, float y, float a) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(RW2PW(x), RW2PW(y));
    bd.angle = a;

    // shape
    float w, h;
    m_pRenderer->GetSize(eSprite::Smallplatform, w, h);
    b2PolygonShape s;
    s.SetAsBox(RW2PW(w) / 2.0f, RW2PW(h) / 2.0f);

    // fixture definition
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 10.0f;
    fd.restitution = 0.1f;

    // create body and fixture
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd);
    m_pObjectManager->CreateObject(eSprite::Smallplatform, pBody);
}

// create ramp
void CGame::CreateRamp(float x, float y, float a) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(RW2PW(x), RW2PW(y));
    bd.angle = a;

    // set polygon vertices
    b2Vec2 triangle[3];
    triangle[0] = setVertice(199.0f, 0.0f, eSprite::Ramp);
    triangle[1] = setVertice(0.0f, 199.0f, eSprite::Ramp);
    triangle[2] = setVertice(199.0f, 199.0f, eSprite::Ramp);

    // create polygon shape
    b2PolygonShape ps;
    ps.Set(triangle, 3);

    // create fixture definition
    b2FixtureDef fd;
    fd.shape = &ps;
    fd.density = 10.0f;
    fd.restitution = 0.3f;
    fd.filter.groupIndex = -10;

    // create body and fixture
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd);
    m_pObjectManager->CreateObject(eSprite::Ramp, pBody);
} 

// create bumper
void CGame::CreateBumper(float x, float y, float a)
{
    // body definition
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(RW2PW(x), RW2PW(y));
    bd.angle = a;

    // shape
    float w, h;
    m_pRenderer->GetSize(eSprite::Bumper, w, h);
    b2PolygonShape s;
    s.SetAsBox(RW2PW(w) / 2.0f, RW2PW(h) / 2.0f);

    // fixture definition
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 10.0f;
    fd.restitution = 2.0f;
    fd.filter.groupIndex = -10;

    // body and fixture
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd);
    m_pObjectManager->CreateObject(eSprite::Bumper, pBody);
}

// create pins
void CGame::CreatePins(float x, float y, float a) {
    // body
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(RW2PW(x), RW2PW(y));

    // set polygon vertices
    b2Vec2 v1[8];
    b2Vec2 v2[8];

    v1[0] = setVertice(7.0f, 74.0f, eSprite::Pin);
    v1[1] = setVertice(0.0f, 56.0f, eSprite::Pin);
    v1[2] = setVertice(0.0f, 48.0f, eSprite::Pin);
    v1[3] = setVertice(8.0f, 22.0f, eSprite::Pin);
    v1[4] = setVertice(16.0f, 22.0f, eSprite::Pin);
    v1[5] = setVertice(24.0f, 48.0f, eSprite::Pin);
    v1[6] = setVertice(24.0f, 56.0f, eSprite::Pin);
    v1[7] = setVertice(18.0f, 74.0f, eSprite::Pin);

    v2[0] = setVertice(8.0f, 21.0f, eSprite::Pin);
    v2[1] = setVertice(5.0f, 14.0f, eSprite::Pin);
    v2[2] = setVertice(5.0f, 8.0f, eSprite::Pin);
    v2[3] = setVertice(10.0f, 0.0f, eSprite::Pin);
    v2[4] = setVertice(14.0f, 0.0f, eSprite::Pin);
    v2[5] = setVertice(19.0f, 8.0f, eSprite::Pin);
    v2[6] = setVertice(19.0f, 14.0f, eSprite::Pin);
    v2[7] = setVertice(16.0f, 21.0f, eSprite::Pin);

    // create polygon shape
    b2PolygonShape ps1;
    b2PolygonShape ps2;
    ps1.Set(v1, 8);
    ps2.Set(v2, 8);

    // create fixture definition
    b2FixtureDef fd1;
    fd1.shape = &ps1;
    fd1.density = 1.0f;
    fd1.restitution = 0.1f;
    b2FixtureDef fd2;
    fd2.shape = &ps2;
    fd2.density = 1.0f;
    fd2.restitution = 0.1f;

    // create body
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd1);
    pBody->CreateFixture(&fd2);

    m_pObjectManager->CreateObject(eSprite::Pin, pBody);
}

// create heavy ball
void CGame::CreateHeavyBall(float x, float y, float d) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(RW2PW(x), RW2PW(y));

    // shape
    b2CircleShape s;
    s.m_radius = RW2PW(m_pRenderer->GetWidth(eSprite::Heavyball)) / 2.0f;

    // fixture
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = d;
    fd.restitution = 0.3f;

    // body
    b2Body* p = m_pPhysicsWorld->CreateBody(&bd);
    p->CreateFixture(&fd);

    m_pObjectManager->CreateObject(eSprite::Heavyball, p);
}

// create propeller
void CGame::CreatePropeller(float x, float y, float a) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(RW2PW(x), RW2PW(y));
    bd.angle = a;

    b2BodyDef bd2;
    bd2.type = b2_staticBody;
    bd2.position.Set(RW2PW(x), RW2PW(y));
    bd2.angle = a;

    // shape
    float w, h;
    m_pRenderer->GetSize(eSprite::Platform, w, h); // propeller has same dimensions as platform. Circle does not need collision
    b2PolygonShape s;
    s.SetAsBox(RW2PW(w) / 2.0f, RW2PW(h) / 2.0f);

    // fixture definition
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 10.0f;
    fd.restitution = 0.001f;
    fd.filter.groupIndex = -10;

    // body and fixture
    b2Body* bodyA = m_pPhysicsWorld->CreateBody(&bd);
    bodyA->CreateFixture(&fd);
    m_pObjectManager->CreateObject(eSprite::Propeller, bodyA);
    b2Body* bodyB = m_pPhysicsWorld->CreateBody(&bd2);

    // revolute joint definition
    b2RevoluteJointDef wd;
    wd.Initialize(bodyB, bodyA, bodyA->GetPosition());
    wd.motorSpeed = 0.0f;
    wd.maxMotorTorque = 4000.0f;
    wd.enableMotor = true;
    wd.collideConnected = false;

    // create revolute joint 
    b2RevoluteJoint* m_pJoint = (b2RevoluteJoint*)m_pPhysicsWorld->CreateJoint(&wd);
}

// create circle bumpers
void CGame::CreateCircleBumpers(float x, float y)
{
    // body definition
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(RW2PW(x), RW2PW(y));

    //shape
    b2CircleShape s;
    s.m_radius = RW2PW(m_pRenderer->GetWidth(eSprite::Circlebumper)) / 2.0f;

    //fixture
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 10.0f;
    fd.restitution = 1.0f;

    //body
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd);

    m_pObjectManager->CreateObject(eSprite::Circlebumper, pBody);
}

// create tower of blocks and sticks
void CGame::CreateTower(float x, float y, eSprite e, float a)
{
    // body definition
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(RW2PW(x), RW2PW(y));
    bd.angle = a;

    // shape
    float w, h;
    m_pRenderer->GetSize(e, w, h);
    b2PolygonShape s;
    s.SetAsBox(RW2PW(w) / 2.0f, RW2PW(h) / 2.0f);

    // fixture definition
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 0.2;
    fd.restitution = 0.5f;
    fd.friction = 1.0f;

    // body
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd);
    m_pObjectManager->CreateObject(e, pBody);
}

// create level
void CGame::CreateLevel()
{
    float w, h; //width and height of pig sprite
    m_pRenderer->GetSize(eSprite::Pig, w, h); //get them
    CreateButton(RW2PW(w) / 2.0f, RW2PW(h) / 2.0f); //create pig

    // top level
    m_pRenderer->GetSize(eSprite::Ramp, w, h);
    CreateRamp(m_nWinWidth - w / 2.0f, m_nWinHeight - h / 2.0f);
    CreateBumper(824, 575);
    CreateRamp(410, m_nWinHeight - h / 2.0f);
    CreateRamp(410 + w, m_nWinHeight - h / 2.0f, -XM_PIDIV2);
    CreatePlatform(220, 574);

    CreatePins(280, 590); // create pins
    CreatePins(245, 590);
    CreatePins(210, 590);
    CreatePins(175, 590);
    CreatePins(140, 590);

    // middle level
    CreateRamp(20, 550, -XM_PIDIV2);
    CreatePlatform(75, 550 - h / 2);
    CreateHeavyBall(150, 525);
    CreatePlatform(240, 381, -XM_PIDIV4);
    CreatePlatform(405, 313);
    CreateSmallPlatform(511, 356, XM_PIDIV2);

    CreatePropeller(405, 430, XM_PIDIV2); // create propeller

    CreateSmallPlatform(556, 440);
    CreateHeavyBall(520, 500);

    m_pPulley = new CPulley(RW2PW(775), RW2PW(275), RW2PW(190)); // create pulley

    CreateSmallPlatform(790, 349, 3.12414);

    m_pBird = new CBird(818, 390); // create bird

    // bottom level
    CreateSmallPlatform(833, 295, XM_PIDIV2);
    CreateSmallPlatform(1016.5, 295, XM_PIDIV2);
    CreateSmallPlatform(855, 205, 5.23599);
    CreateSmallPlatform(1000, 205, 4.27606);

    CreateCircleBumpers(870, 310); // create circle bumpers
    CreateCircleBumpers(985, 310);
    CreateCircleBumpers(927.5, 260);
    CreateCircleBumpers(875, 210);
    CreateCircleBumpers(985, 210);

    m_pCatapult = new CCatapult(RW2PW(890), RW2PW(50)); // create catapult

    m_pRenderer->GetSize(eSprite::Block, w, h); // create tower
    CreateTower(200, 83, eSprite::Stick, XM_PIDIV2);
    CreateTower(300, 83, eSprite::Stick, XM_PIDIV2);
    CreateTower(250, 175, eSprite::Stick);

    CreateTower(250, 205, eSprite::Block);
    CreateTower(250 - w, 205, eSprite::Block);
    CreateTower(250 + w, 205, eSprite::Block);

    CreateTower(250 - w / 2.0f, 205 + h, eSprite::Block);
    CreateTower(250 + w / 2.0f, 205 + h, eSprite::Block);
    CreateTower(250, 205 + 2 * h, eSprite::Block);

    return;
}