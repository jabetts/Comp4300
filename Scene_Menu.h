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
	float								m_fontSpaceVel = 0.1f;
	float								m_currentSpace = 1.0f;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action & action) override;

public:

	Scene_Menu();
	Scene_Menu(GameEngine* gameEngine);
	void sRender();
};

