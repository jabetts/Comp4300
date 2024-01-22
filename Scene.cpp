#include "Scene.h"

void Scene::simulate(int i)
{
}

//void Scene::registerAction(Action action)
//{
//}

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

size_t Scene::width() const
{
	return 64;
}

size_t Scene::height() const
{
	return 64;
}

size_t Scene::currentFrame() const
{
	return 0;
}

bool Scene::hasEnded() const
{
	return false;
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2)
{
}
