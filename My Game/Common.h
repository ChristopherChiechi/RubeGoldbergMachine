/// \file Common.h
/// \brief Interface for the class CCommon.

#ifndef __L4RC_GAME_COMMON_H__
#define __L4RC_GAME_COMMON_H__

#include "GameDefines.h"
#include "ParticleEngine.h"

//forward declarations to make the compiler less stroppy

class CObjectManager;
class CRenderer;
class b2World;

class CPulley;
class CCatapult;
class CBird;

/// \brief The common variables class.
///
/// CCommon is a singleton class that encapsulates 
/// things that are common to different game components,
/// including game state variables.
/// Making it a singleton class means
/// that we can avoid passing its member variables
/// around as parameters, which makes the code
/// minisculely faster, and more importantly, reduces
/// function clutter.

class CCommon{
  protected:   
    static b2World* m_pPhysicsWorld; ///< Pointer to Box2D Physics World.
    static CRenderer* m_pRenderer; ///< Pointer to renderer.
    static CObjectManager* m_pObjectManager; ///< Pointer to object manager.
    static LParticleEngine2D* m_pParticleEngine; ///< Pointer to particle engine.
    
    static float m_fStartTime; ///< Time machine started.
    static float m_fTotalTime; ///< Elapsed time at finish.

    static eGameState m_eGameState; ///< Game state.
    static eDrawMode m_eDrawMode;  ///< Draw mode.

    static CPulley* m_pPulley; ///< Pointer to pulley system.
    static CCatapult* m_pCatapult; ///< Pointer to car.
    static CBird* m_pBird;
}; //CCommon

#endif //__L4RC_GAME_COMMON_H__
