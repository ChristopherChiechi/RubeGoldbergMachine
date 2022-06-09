#pragma once
#include "Object.h"
#include "Box2D\Box2D.h"

#include "Common.h"
#include "Component.h"

class CPulley :
    public CCommon,
    public LComponent {

private:
    b2PulleyJoint* m_pJoint = nullptr; ///< Pointer to the pulley joint.
    float m_fJointLenA = 0; ///< Length of left side of pulley.

    b2Body* m_pWheel0 = nullptr; ///< Pointer to the left pulley wheel.
    b2Body* m_pWheel1 = nullptr; ///< Pointer to the right pulley wheel.
    float m_fWheelRad = 1; ///< Pulley wheel radius.

    b2Body* CreateWheel(float, float); ///< Create a pulley wheel.
    b2Body* CreateBasket(float, float); ///< Create basket.

public:
    CPulley(float, float, float); ///< Constructor.

    void move(); ///< Rotate the pulley wheels.
}; //CPulley