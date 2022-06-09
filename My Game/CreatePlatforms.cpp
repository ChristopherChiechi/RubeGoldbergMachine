#include "CreatePlatforms.h"

#include "GameDefines.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "ComponentIncludes.h"

void CCreatePlatforms::CreateRamp()
{
	b2BodyDef bd;
	bd.type = b2_staticBody;
	bd.position.Set(RW2PW(m_nWinWidth), RW2PW(m_nWinHeight));

	b2PolygonShape ps;
	//ps.Set


	b2FixtureDef fd;
	fd.shape = &ps;
	fd.density = 1.0f;
	fd.restitution = 0.3f;

	b2Body* pBody = m_pPhysicsWorld->CreateBody(&bd);
	pBody->CreateFixture(&fd);
	m_pObjectManager->CreateObject(eSprite::Ramp, pBody);
}