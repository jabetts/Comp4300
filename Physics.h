#pragma once

#include "EntityManager.h"

class Physics
{
	Physics() {}

	Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	bool isCollision(Entity a, Entity b) const;

};

