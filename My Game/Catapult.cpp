#include "Catapult.h"

#include "GameDefines.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "ComponentIncludes.h"
#include "Bird.h"

// constructor
CCatapult::CCatapult(float x, float y)
{
    const float height = m_pRenderer->GetHeight(eSprite::Base);
    const float width = m_pRenderer->GetWidth(eSprite::Base);

    // create body, catapult, and wheels
    m_pBody = CreateBody(x, y);
    m_pCatapult = CreateCatapult(x, y + RW2PW(height + 15));
    b2Body* pWheel1 = CreateWheel(x - RW2PW(width / 2.0f - 30), y - RW2PW(height / 2.0f - 5));
    b2Body* pWheel2 = CreateWheel(x + RW2PW(width / 2.0f - 30), y - RW2PW(height / 2.0f - 5));

    b2Vec2 axis(0.0f, 1.0f); // vertical axis for wheel suspension

    // wheel joint definition
    b2WheelJointDef wd;
    wd.Initialize(m_pBody, pWheel1, pWheel1->GetPosition(), axis);
    wd.motorSpeed = 0.0f;
    wd.maxMotorTorque = 1000.0f;
    wd.enableMotor = true;
    wd.damping = 0.1f;
    wd.stiffness = 999.0f;
    wd.collideConnected = false;
    
    // create wheel joint
    m_pWheelJoint1 = (b2WheelJoint*)m_pPhysicsWorld->CreateJoint(&wd);
    wd.Initialize(m_pBody, pWheel2, pWheel2->GetPosition(), axis);
    m_pWheelJoint2 = (b2WheelJoint*)m_pPhysicsWorld->CreateJoint(&wd);

    // revolute joint definition
    b2RevoluteJointDef jd;
    jd.Initialize(m_pCatapult, m_pBody, m_pCatapult->GetPosition());
    jd.maxMotorTorque = 1000.0f;
    jd.motorSpeed = 0.0f;
    jd.enableMotor = true;
    jd.upperAngle = 0.0f;
    jd.enableLimit = true;

    //create revolute joint
    m_pCatapultJoint = (b2RevoluteJoint*)m_pPhysicsWorld->CreateJoint(&jd);
}

// create body
b2Body* CCatapult::CreateBody(float x, float y)
{
    float w, h;
    m_pRenderer->GetSize(eSprite::Base, w, h);
    const float w2 = RW2PW(w) / 2.0f;
    const float h2 = RW2PW(h) / 2.0f;

    //shape
    b2Vec2 vertices[3];
    vertices[0].Set(-w2, -h2);
    vertices[1].Set(w2, -h2);
    vertices[2].Set(0.0f, h2);

    b2PolygonShape shape;
    shape.Set(vertices, 3);

    //fixture
    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 1.0f;
    fd.restitution = 0.4f;
    fd.filter.groupIndex = -5;

    //body definition
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, y + h2);

    //body
    b2Body* body = m_pPhysicsWorld->CreateBody(&bd);
    m_pObjectManager->CreateObject(eSprite::Base, body);
    body->CreateFixture(&fd);

    return body;
}

// create wheel
b2Body* CCatapult::CreateWheel(float x, float y)
{
    const float r = m_pRenderer->GetWidth(eSprite::Wheel) / 2.0f;

    //shape
    b2CircleShape shape;
    shape.m_radius = RW2PW(r);

    //fixture
    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 0.8f;
    fd.restitution = 0.6f;
    fd.filter.groupIndex = -5;

    //body definition
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, y);

    //body
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    m_pObjectManager->CreateObject(eSprite::Wheel, pBody);
    pBody->CreateFixture(&fd);

    return pBody;
}

// move catapult
void CCatapult::move()
{
    const float x = PW2RW(m_pBody->GetPosition().x);

    // if catapult is to the right of the screen's center,
    if (x >= m_vWinCenter.x)
    {
        // set motor speed to 4
        m_pWheelJoint1->SetMotorSpeed(4.0f);
        m_pWheelJoint2->SetMotorSpeed(4.0f);
    }
    else
    {
        // set motor speed to 0, stopping catapult
        m_pWheelJoint1->SetMotorSpeed(0.0f);
        m_pWheelJoint2->SetMotorSpeed(0.0f);

        // rotate catapult for 60 frames (i.e. 1 second)
        if (counter <= 59)
        {
            Rotate();
            counter++;
        }

        // launch bird
        m_pBird->move();
    }
}

// create catapult
b2Body* CCatapult::CreateCatapult(float x, float y)
{
    // body
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, y);
    bd.angle = 6.10865;

    // set polygon vertices
    b2Vec2 v1[4];
    b2Vec2 v2[4];

    v1[0] = setVertice(0.0f, 65.0f, eSprite::Catapult);
    v1[1] = setVertice(159.0f, 65.0f, eSprite::Catapult);
    v1[2] = setVertice(159.0f, 79.0f, eSprite::Catapult);
    v1[3] = setVertice(0.0f, 79.0f, eSprite::Catapult);

    v2[0] = setVertice(142.0f, 0.0f, eSprite::Catapult);
    v2[1] = setVertice(159.0f, 0.0f, eSprite::Catapult);
    v2[2] = setVertice(159.0f, 79.0f, eSprite::Catapult);
    v2[3] = setVertice(142.0f, 79.0f, eSprite::Catapult);

    // create polygon shape
    b2PolygonShape ps1;
    b2PolygonShape ps2;
    ps1.Set(v1, 4);
    ps2.Set(v2, 4);

    // create fixture definition
    b2FixtureDef fd1;
    fd1.shape = &ps1;
    fd1.density = 1.0f;
    fd1.restitution = 0.5f;
    fd1.filter.groupIndex = -5;
    b2FixtureDef fd2;
    fd2.shape = &ps2;
    fd2.density = 1.0f;
    fd2.restitution = 0.0f;
    fd2.filter.groupIndex = -5;

    // create body and fixture
    b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
    pBody->CreateFixture(&fd1);
    pBody->CreateFixture(&fd2);

    m_pObjectManager->CreateObject(eSprite::Catapult, pBody);

    return pBody;
}

// set the vertices of polygons
b2Vec2 CCatapult::setVertice(float x, float y, eSprite e)
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

// rotate catapult arm
void CCatapult::Rotate()
{
    m_pCatapult->SetTransform(m_pCatapult->GetPosition(), m_pCatapult->GetAngle() + 0.11);
}

// get collision between bird and catapult
bool CCatapult::GetCollision() {
    return collision;
}

// set collision between bird and catapult
void CCatapult::SetCollision(bool c) {
    collision = c;
}