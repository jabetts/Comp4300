#include "Physics.h"
#include "components.h"

#include <iostream>

Vec2 Physics::GetOverlap(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	// if both entity's have transforms and bounding boxes, then compute the overlap

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
	// TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
	//       previous overlap uses the entity's previous position
	//       same as above only using prev position

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

	// TODO: May be able to speed this up by checking each axis separatley and returning false if either
	// fail 
	return(p1.x < p2.x + s2.x && p2.x < p1.x + s1.x &&
		p1.y < p2.y + s2.y && p2.y < p1.y + s1.y);
}
