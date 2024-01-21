#include "Scene_Menu.h"
#include "Scene_Play.h"

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W,			"UP");
	registerAction(sf::Keyboard::S,			"DOWN");
	registerAction(sf::Keyboard::D,			"PLAY");
	registerAction(sf::Keyboard::Escape,	"QUIT");

	m_entityManager = EntityManager();
	m_title = "Menu";
	m_menuStrings.push_back("Level 1");
	m_menuStrings.push_back("Level 2");
	m_menuStrings.push_back("Level 3");

	m_levelPaths.push_back("level1.txt");
	m_levelPaths.push_back("level1.txt");
	m_levelPaths.push_back("level1.txt");

	m_menuText.setFont(m_game->assets().getFont("Hack"));
	m_menuText.setString(m_menuStrings[0]);
	m_menuText.setCharacterSize(72);

	update();
}

Scene_Menu::Scene_Menu()
{
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	m_game = gameEngine;
	init();
}

void Scene_Menu::sRender()
{
	m_game->window().clear(sf::Color(33,57,125,255));

	for (auto s : m_menuStrings)
	{
		//TODO: 
		// Change the m_menuText object for each menu entry

	}

	m_game->window().display();

	m_currentFrame++;
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
			else { m_menuIndex - m_menuStrings.size() - 1; }
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			m_game->changeScene("play", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::onEnd()
{
	m_game->window().close();
}
