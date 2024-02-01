#include "Scene.h"

void Scene::simulate(int i)
{
}

void Scene::setPaused(bool paused)
{
}

Scene::Scene()
{
}

Scene::Scene(GameEngine* gameEngine)
{
	m_game = gameEngine;
}

void Scene::doAction(const Action& action)
{
	sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
}

void Scene::registerAction(int inputKey,const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

bool Scene::hasEnded() const
{
	return false;
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

sf::VertexArray Scene::drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0].position = p1;
	line[1].position = p2;
	line[0].color = sf::Color::White;
	line[1].color = sf::Color::White;

	return line;
}
