#pragma once
#include "GameDefines.h"
#include "Box2D\Box2D.h"

#include "Common.h"
#include "Settings.h"
#include "Component.h"
#include "Object.h"

class CBird :
    public CCommon,
    public LComponent,
    public LSettings {

private:
    b2Body* pBird = nullptr; // bird body
    bool launched = false; // flag for if bird is launched
    
public:
    CBird(float, float); // constructor
    void move(); // move bird
    void DeliverImpulse(b2Body*, const b2Vec2&, const b2Vec2 & = b2Vec2(0, 0)); // deliver impulse to bird
}; 