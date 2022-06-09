#pragma once
#include "Object.h"
#include "Box2D\Box2D.h"
#include "Common.h"
#include "Component.h"
#include "Settings.h"

class CCatapult :
    public CCommon,
    public LSettings,
    public LComponent
{
private:
    int counter = 0; // frame counter
    bool collision = false; // flag for collision with bird and catapult

    b2Body* m_pBody = nullptr; // pointer to body
    b2Body* m_pCatapult = nullptr; // pointer to catapult

    b2WheelJoint* m_pWheelJoint1 = nullptr; // ointer to wheel joint
    b2WheelJoint* m_pWheelJoint2 = nullptr; // pointer to wheel joint
    b2RevoluteJoint* m_pCatapultJoint = nullptr; // pointer to catapult joint

    b2Body* CreateBody(float, float); // create body
    b2Body* CreateWheel(float, float); // create wheel
    b2Body* CreateCatapult(float, float);

    b2Vec2 setVertice(float, float, eSprite); // set the vertices of polygons
    void Rotate(); // rotate catapult arm

public:
    CCatapult(float, float); // constructor

    void move(); // Moves and rotates catapult. Is called after bird collides with catapult
    bool GetCollision(); // get collision between bird and catapult
    void SetCollision(bool); // set collision between bird and catapult
}; //CCatapult