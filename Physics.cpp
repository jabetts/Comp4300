#include "Physics.h"
#include "components.h"

#include <iostream>

Vec2 Physics::GetOverlap(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b)
{
	auto& posa = a->getComponent<CTransform>().pos;
	auto& posb = b->getComponent<CTransform>().pos;
	auto& boxa = a->getComponent<CBoundingBox>();
	auto& boxb = b->getComponent<CBoundingBox>();

	Vec2 delta(std::abs(posa.x - posb.x),std::abs(posa.y - posb.y));
	float ox = (boxa.halfSize.x + boxb.halfSize.x) - delta.x;
	float oy = (boxa.halfSize.y + boxb.halfSize.y) - delta.y;

	return Vec2(ox, oy);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& posa = a->getComponent<CTransform>().pos;
	auto& posb = b->getComponent<CTransform>().prevPos;
	auto& boxa = a->getComponent<CBoundingBox>();
	auto& boxb = b->getComponent<CBoundingBox>();

	Vec2 delta(std::abs(posa.x - posb.x), std::abs(posa.y - posb.y));
	float ox = (boxa.halfSize.x + boxb.halfSize.x) - delta.x;
	float oy = (boxa.halfSize.y + boxb.halfSize.y) - delta.y;

	return Vec2(ox, oy);
}

bool Physics::isCollision(const std::shared_ptr<Entity> a,const std::shared_ptr<Entity> b) const
{
	Vec2 p1 = a->getComponent<CTransform>().pos - a->getComponent<CBoundingBox>().halfSize;
	Vec2 p2 = b->getComponent<CTransform>().pos - b->getComponent<CBoundingBox>().halfSize;
	
	Vec2& s1 = a->getComponent<CBoundingBox>().size;
	Vec2& s2 = b->getComponent<CBoundingBox>().size;

	return(p1.x < p2.x + s2.x && p2.x < p1.x + s1.x &&
		p1.y < p2.y + s2.y && p2.y < p1.y + s1.y);
}

// This function checks if any point of the bullet Bounding Box is and entity
bool Physics::isPointCollision(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b) const
{
	// check if location of the bullet (b) is inside entity (e)
	auto bPos = b->getComponent<CTransform>().pos;
	auto aPos = a->getComponent<CTransform>().pos;
	auto aLen = a->getComponent<CBoundingBox>().halfSize;

	return ((bPos.x > aPos.x - aLen.x && bPos.x < aPos.x + aLen.x) &&
		    (bPos.y > aPos.y - aLen.y && bPos.y < aPos.y + aLen.y));
}