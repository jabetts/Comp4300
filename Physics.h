#pragma once

#include "EntityManager.h"

class Physics
{
public:
	Physics() {}

	Vec2 GetOverlap(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b);
	Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	bool isCollision(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b) const;
	bool isPointCollision(const std::shared_ptr<Entity> a, const std::shared_ptr<Entity> b) const;


};

