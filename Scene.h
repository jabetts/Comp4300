#pragma once
#include <map>
#include <string>

#include <SFML/Graphics.hpp>

#include "Action.h"
#include "EntityManager.h"

class GameEngine;

typedef std::map<int, std::string>	ActionMap;

class Scene
{

protected:

	GameEngine*							m_game = nullptr;
	EntityManager						m_entityManager;
	ActionMap							m_actionMap;
	bool								m_hasEnded = false;
	bool								m_paused = false;
	size_t								m_currentFrame = 0;
	virtual void onEnd() = 0;
	void setPaused(bool paused);

public:

	Scene();
	Scene(GameEngine* gameEngine);

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	virtual void doAction(const Action& action);
	void simulate(const size_t frames);
	void registerAction(int inputKey, const std::string& actionName);
	void simulate(int i);

	size_t currentFrame() const;

	bool hasEnded() const;
	const ActionMap& getActionMap() const;
	sf::VertexArray drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2);
};
