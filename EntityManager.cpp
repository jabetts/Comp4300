#include "EntityManager.h"
#include <iostream>


EntityManager::EntityManager()
{

}

void EntityManager::removeDeadEntities(EntityVec& Vec)
{
	// remove all dead entities from the input vector
	// this is called by the update() function
	auto erased = std::erase_if(Vec, [](auto& e) {
		return !e->isActive();});
}

void EntityManager::update()
{
	// TODO: add entities from the m_entitiesToAdd the proper locaction(s)
	//       - add the to the vector of all entitities
	//       - add them to the vector inside the map, with the tag as a key
	for (auto & e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();
	
	// remove dead entities from the vector of all entitities
	// c++17 way of iterating through [key,value] pairs in a map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}


std::shared_ptr<Entity> EntityManager::addEntity(std::string tag)
{	
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec & EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec & EntityManager::getEntities(const std::string & tag)
{
	return m_entityMap[tag];
}



