#include "Physics.h"
#include "components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	// if both entity's have transforms and bounding boxes, then computre the overlap

	auto& pos = a->getComponent<CTransform>().pos;

	return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
	//       previous overlap uses the entity's previous position
	//       same as above only using prev position

	return Vec2(0, 0);
}

bool Physics::isCollision(Entity a, Entity b) const
{
	return false;
}
