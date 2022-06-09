#include "Bird.h"

#include "GameDefines.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "ComponentIncludes.h"

// constructor
CBird::CBird(float x, float y)
{
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(RW2PW(x), RW2PW(y));

    //shape
    b2CircleShape s;
    s.m_radius = RW2PW(m_pRenderer->GetWidth(eSprite::Bird)) / 2.0f;

    //fixture
    b2FixtureDef fd;
    fd.shape = &s;
    fd.density = 1.0f;
    fd.restitution = 0.1f;

    //body and fixture
    pBird = m_pPhysicsWorld->CreateBody(&bd);
    pBird->CreateFixture(&fd);

    m_pObjectManager->CreateObject(eSprite::Bird, pBird);
}

// move bird
void CBird::move()
{
        // upward direction in physics world
        const b2Vec2 v0(-900.0f, 900.0f);

        // position in physics world to apply impulse
        const b2Vec2 p0(pBird->GetPosition().x, pBird->GetPosition().y);

        // deliver impulse to bird
        if (launched == false)
        {
            m_pBird->DeliverImpulse(pBird, v0, p0);
            launched = true;
        }
}

// deliver impulse
void CBird::DeliverImpulse(b2Body* b, const b2Vec2& v, const b2Vec2& ds)
{
    b->ApplyLinearImpulse(v, b->GetPosition() , true);
}