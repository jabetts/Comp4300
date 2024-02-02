#pragma once

#include "vec2.h"
#include "Animation.h"
#include <SFML\Graphics.hpp>


struct Component
{
	bool has = false;
};

struct CDraggable : public Component
{
	bool dragging = false;
	CDraggable() {}
	CDraggable(bool dragging) : dragging(dragging) {}
};

struct CTransform : public Component
{
	Vec2 pos		= { 0.0, 0.0 };
	Vec2 prevPos    = { 0.0, 0.0 };
	Vec2 scale      = { 1.0, 1.0 };
	Vec2 velocity	= { 0.0, 0.0 };
	float angle		= 0;

	CTransform() {}
	CTransform(const Vec2& p)
		:pos(p) {}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2 & sc, float a)
		:pos(p), velocity(sp), scale(sc), angle(a) {}
};

struct CScore
{
	int score = 0;
	CScore(int s)
		: score(s) {}
};

struct CLifeSpan : Component
{
	int lifespan	 = 0;
	int frameCreated = 0;
	CLifeSpan() {}
	CLifeSpan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

struct CInput : Component
{
	bool up		  = false;
	bool left	  = false;
	bool right	  = false;
	bool down	  = false;
	bool shoot	  = false;
	bool canShoot = true;
	bool canJump  = true;

	CInput() {}
};

struct CAttributes
{
	float gravity = 0 ;
	float jumpTime= 0;
	float walkAirSpeed = 0;
	float maxSpeed = 0;
	float maxFallSpeed = 0;
	bool animLoop = true;

	CAttributes() {}
};

struct CBoundingBox : public Component
{
	Vec2 size;
	Vec2 halfSize;
	int height = 0;
	
	CBoundingBox() {}
	CBoundingBox(const Vec2 & s)
		: size(s), halfSize(s.x / 2, s.y / 2) {}
};

struct CAnimation : public Component
{
	Animation animation;
	bool repeat = false;
	CAnimation(){}
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}
};

struct CGravity : public Component
{
	float gravity = 0;
	CGravity() {}
	CGravity(float g): gravity(g) {}

};

struct CState : public Component
{
	std::string state = "none";
	CState() {}
	CState(const std::string& s) : state(s) {}
};

