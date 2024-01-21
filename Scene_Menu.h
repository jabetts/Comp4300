#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <deque>
#include "GameEngine.h"
#include "Scene.h"
#include "EntityManager.h"

class Scene_Menu : public Scene
{
protected:
	std::string							m_title;
	std::vector<std::string>			m_menuStrings;
	std::vector<std::string>			m_levelPaths;
	sf::Text							m_menuText;
	size_t								m_selectedMenuIndex = 0;
	int									m_menuIndex = 0;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action & action);

public:

	Scene_Menu();
	Scene_Menu(GameEngine* gameEngine);
	void sRender();
};

