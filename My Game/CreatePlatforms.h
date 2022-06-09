#include "Object.h"
#include "Box2D\Box2D.h"

#include "Component.h"
#include "Common.h"
#include "Settings.h"

class CCreatePlatforms :
    public LComponent,
    public CCommon,
    public LSettings
{
private:
    //b2RevoluteJoint* m_pJoint = nullptr; ///< Pointer to revolute joint.
    //b2Body* CreateBlade(float, float); ///< Create the blades.
    //b2Body* CreateBase(float, float); ///< Create the base.
    //b2Vec2 setVertice(float, float); // set the vertices of polygons
    void CreateRamp();

public:
    //CWindmill(float, float); ///< Constructor.
    //void reverse(); ///< Reverse motor.
}; //CWindmill#pragma once
