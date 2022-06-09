/// \file Game.h
/// \brief Interface for the game class CGame.

#ifndef __L4RC_GAME_GAME_H__
#define __L4RC_GAME_GAME_H__

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"

#include "ContactListener.h"

/// \brief The game class.

class CGame: 
  public LComponent, //game components from the Engine
  public LSettings, //game settings from gamesettings.xml, via the Engine
  public CCommon{ //common parameters for this game

  private: 
    CMyListener m_cContactListener; ///< Contact listener.
    void LoadSounds(); ///< Load sounds. 

    void BeginGame(); ///< Begin playing the game.
    void KeyboardHandler(); ///< The keyboard handler.
    void DrawClock(); ///< Draw a timer.
    void RenderFrame(); ///< Render an animation frame.

    void CreateButton(float x, float y); ///< Create final button.
    void CreateBall(float x, float y, float xv=0.0f, float yv=0.0f); ///< Create and launch ball.
    void CreateHeavyBall(float x, float y, float d = 10.0f); // create heavyball
    void CreatePlatform(float x, float y, float a = XM_2PI); // create platform
    void CreateSmallPlatform(float x, float y, float a = XM_2PI); // create small platform
    void CreateRamp(float x, float y, float a = XM_2PI); // create ramp
    void CreateBumper(float x, float y, float a = XM_2PI); // create bumper
    void CreatePins(float x, float y, float a = XM_2PI); // create pins
    void CreatePropeller(float x, float y, float a = XM_2PI); // create propeller
    void CreateCircleBumpers(float x, float y); // create circle bumpers
    void CreateTower(float x, float y, eSprite e, float a = XM_2PI); // create tower of blocks and sticks
    void CreateLevel(); // create level

    b2Vec2 setVertice(float, float, eSprite); // set the vertices of polygons

  public:
    ~CGame(); ///< Destructor.

    void Initialize(); ///< Initialize the game.
    void ProcessFrame(); ///< Process an animation frame.
    void Release(); ///< Release the renderer.
}; //CGame

#endif //__L4RC_GAME_GAME_H__
