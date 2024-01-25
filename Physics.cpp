#include "Physics.h"
#include "components.h"

#include <iostream>

Vec2 Physics::GetOverlap(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	// if both entity's have transforms and bounding boxes, then computre the overlap

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
	int x1 = a->getComponent<CTransform>().pos.x;
	int x2 = b->getComponent<CTransform>().pos.x;
	int y1 = a->getComponent<CTransform>().pos.y;
	int y2 = b->getComponent<CTransform>().pos.y;
	int w1 = a->getComponent<CBoundingBox>().size.x;
	int w2 = b->getComponent<CBoundingBox>().size.x;
	int h1 = a->getComponent<CBoundingBox>().size.y;
	int h2 = b->getComponent<CBoundingBox>().size.y;

	// Vertical overlaps if x1 < x2 + w2 && x2 < x1 + w1
	// Horizontal overlap if y1 < y2 + h2 && y2 < y1 + h1;
	return ((x1 < x2 + w2 && x2 < x1 + w1) && (y1 < y2 + h2 && y2 < y1 + h1));
}
