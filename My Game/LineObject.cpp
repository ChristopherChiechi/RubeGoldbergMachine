#include "LineObject.h"
#include "Renderer.h"
#include "ComponentIncludes.h"

// constructor
CLineObject::CLineObject(
    b2Body* b0, b2Vec2 d0, bool r0,
    b2Body* b1, b2Vec2 d1, bool r1) :

    m_pBody0(b0), m_vAnchor0(d0), m_bRotates0(r0),
    m_pBody1(b1), m_vAnchor1(d1), m_bRotates1(r1) {
} //constructor

/// Draw in Render World. This line goes from anchor 0 on 
/// body 0 to anchor 1 on body 1 in Physics World.

void CLineObject::draw() {
    b2Vec2 d0 = m_vAnchor0; //offset to anchor 0 from body 0 center
    b2Vec2 d1 = m_vAnchor1; //offset to anchor 1 from body 1 center

    if (m_bRotates0) //if anchor 0 rotates with body 0
        d0 = b2Mul(b2Rot(m_pBody0->GetAngle()), d0); //rotate its offset

    if (m_bRotates1) //if anchor 1 rotates with body 1
        d1 = b2Mul(b2Rot(m_pBody1->GetAngle()), d1); //rotate its offset

    const Vector2 a0 = PW2RW(m_pBody0->GetPosition() + d0); //anchor 0 position in Render World
    const Vector2 a1 = PW2RW(m_pBody1->GetPosition() + d1); //anchor 1 position in Render World

    m_pRenderer->DrawLine(eSprite::Pulleyline, a0, a1); //now draw the line
} //draw