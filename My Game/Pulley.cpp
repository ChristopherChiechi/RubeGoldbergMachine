#include "Pulley.h"

#include "GameDefines.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "ComponentIncludes.h"

/// \param x X coordinate in Physics World units.
/// \param y Y coordinate in Physics World units.
/// \param w Pulley wheel horizontal separation in Physics World units.

CPulley::CPulley(float x, float y, float w) {
    //crate calculations
    const float fCrateWidth = m_pRenderer->GetWidth(eSprite::Basket); //crate width in Render World
    const float fCrateHt = m_pRenderer->GetHeight(eSprite::Basket); //crate height in Render World
    const float fCrateWidth2 = RW2PW(fCrateWidth) / 2.0f; //crate half width in Physics World
    const float fCrateHt2 = RW2PW(fCrateHt) / 2.0f; //crate half height in Physics World

    //pulley wheel calculations
    const float fWheelDiam = m_pRenderer->GetWidth(eSprite::Pulleywheel); //pulley wheel diameter in Render World
    m_fWheelRad = RW2PW(fWheelDiam) / 2.0f - RW2PW(4); //pulley wheel radius in Physics World
    const float fWheelSep2 = w / 2.0f; //half pulley wheel separation in Physics World
    const float fWheelAlt = 2.0f * (y - 1.2f * m_fWheelRad); //wheel altitude on screen

     //calculate positions
    const b2Vec2 vCratePos0 = b2Vec2(x - fWheelSep2 - m_fWheelRad, RW2PW(350));
    const b2Vec2 vCratePos1 = b2Vec2(x + fWheelSep2 + m_fWheelRad, RW2PW(360));
    const b2Vec2 vWheelPos0 = b2Vec2(x - fWheelSep2, fWheelAlt);
    const b2Vec2 vWheelPos1 = b2Vec2(x + fWheelSep2, fWheelAlt);

    //create bodies
    b2Body* pCrate0 = CreateBasket(vCratePos0.x, vCratePos0.y);
    b2Body* pCrate1 = CreateBasket(vCratePos1.x, vCratePos1.y);
    m_pWheel0 = CreateWheel(vWheelPos0.x, vWheelPos0.y);
    m_pWheel1 = CreateWheel(vWheelPos1.x, vWheelPos1.y);

    //calculate anchor points
    const b2Vec2 vCrateAnchor0 = vCratePos0 + b2Vec2(0.0f, fCrateHt2);
    const b2Vec2 vCrateAnchor1 = vCratePos1 + b2Vec2(0.0f, fCrateHt2);
    const b2Vec2 vWheelAnchor0 = vWheelPos0 - b2Vec2(m_fWheelRad, 0.0f);
    const b2Vec2 vWheelAnchor1 = vWheelPos1 + b2Vec2(m_fWheelRad, 0.0f);

    //create pulley joint
    b2PulleyJointDef jd;
    jd.Initialize(pCrate0, pCrate1, //bodies
        vWheelAnchor0, vWheelAnchor1, //anchors on wheels
        vCrateAnchor0, vCrateAnchor1, //anchors on bodies
        1.0f);

    m_pJoint = (b2PulleyJoint*)m_pPhysicsWorld->CreateJoint(&jd);
    m_fJointLenA = m_pJoint->GetCurrentLengthA();

    //create lines to represent the rope
    //rope from wheel0 to crate0
    b2Vec2 offset0 = b2Vec2(-m_fWheelRad, 0.0f);
    b2Vec2 offset1 = b2Vec2(0.0f, 0.0f);//fCrateHt2 - RW2PW(4));
    m_pObjectManager->CreateLine(m_pWheel0, offset0, false, pCrate0, offset1, true);

    //rope from wheel1 to crate1
    offset0 = b2Vec2(m_fWheelRad, 0.0f);
    offset1 = b2Vec2(0.0f, 0.0f);
    m_pObjectManager->CreateLine(m_pWheel1, offset0, false, pCrate1, offset1, true);

    //rope from one pulley wheel across to the other
    offset0 = b2Vec2(0.0f, m_fWheelRad);
    m_pObjectManager->CreateLine(m_pWheel0, offset0, false, m_pWheel1, offset0, false);
} //constructor

/// The pulley wheels are going to be static objects that we
/// rotate ourselves depending on the positions of the objects
/// that the pulley is attached to.
/// \param x X coordinate in Physics World units.
/// \param y Y coordinate in Physics World units.
/// \return Pointer to physics body.

b2Body* CPulley::CreateWheel(float x, float y) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(x, y);

    // create body
    b2Body* p = m_pPhysicsWorld->CreateBody(&bd);
    m_pObjectManager->CreateObject(eSprite::Pulleywheel, p);

    return p;
} //CreateWheel

/// \param x X coordinate in Physics World units.
/// \param y Y coordinate in Physics World units.
/// \return Pointer to physics body.

b2Body* CPulley::CreateBasket(float x, float y) {
    // body definition
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, y);

    // create body
    b2Body* p = m_pPhysicsWorld->CreateBody(&bd);
    m_pObjectManager->CreateObject(eSprite::Basket, p);

    float cw = RW2PW(m_pRenderer->GetWidth(eSprite::Basket) / 2.0f); //crate half width 
    float ch = RW2PW(m_pRenderer->GetHeight(eSprite::Basket) / 2.0f); //crate half height 
    const float sh = RW2PW(5) / 2.0f; //half height of crate floor

    // shape and fixture definition
    b2PolygonShape s; //shape for basket floor and walls
    b2FixtureDef fd; //fixture definition
    fd.shape = &s;
    fd.density = 900.0f;
    fd.restitution = 0.0f;

    s.SetAsBox(cw, sh, b2Vec2(0.0f, sh - ch), 0.0f); //bottom of crate
    p->CreateFixture(&fd);

    s.SetAsBox(sh, ch, b2Vec2(-cw + sh, 0.0f), 0.0f); //left wall
    p->CreateFixture(&fd);

    s.SetAsBox(sh, ch, b2Vec2(cw - sh, 0.0f), 0.0f); //right wall

    // create fixture
    p->CreateFixture(&fd);
    p->SetLinearDamping(0.2f);
    p->SetAngularDamping(0.1f);

    return p;
}

/// Animate the pulley wheels by setting their orientations
/// to reflect the amount of rope that has gone over them.

void CPulley::move() {
    const float fLenA = m_pJoint->GetCurrentLengthA(); //current length of left rope
    const float theta = (fLenA - m_fJointLenA) / m_fWheelRad; //new wheel orientation

    m_pWheel0->SetTransform(m_pWheel0->GetPosition(), theta); // left wheel
    m_pWheel1->SetTransform(m_pWheel1->GetPosition(), theta + 2.4f); // slighty offset right wheel
} //move
