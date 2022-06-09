#pragma once
#include "Common.h"
#include "Component.h"

/// \brief A line in object manager.
///
/// A line object differs from an ordinary object in that it is
/// associated with two bodies in Physics World instead of just one.
/// The line goes from an anchor on one body to an anchor on
/// another body. AN anchor is just a vector offset from the
/// center of the body in Physics World. The anchors may or 
/// may not rotate with their associated bodies.

class CLineObject :
    public CCommon,
    public LComponent
{
private:
    b2Body* m_pBody0 = nullptr; ///< Pointer to body0 in Physics World.
    b2Vec2 m_vAnchor0; ///< Anchor on body0 in Physics World.
    bool m_bRotates0 = false; ///< Whether anchor rotates with body0.

    b2Body* m_pBody1 = nullptr; ///< Pointer to body1 in Physics World.
    b2Vec2 m_vAnchor1; ///< Anchor on body1 in Physics World.
    bool m_bRotates1 = false; ///< Whether anchor rotates with body1.

public:
    CLineObject(b2Body*, b2Vec2, bool, b2Body*, b2Vec2, bool); ///< Constructor.

    void draw(); ///< Draw line.
}; //CLineObject