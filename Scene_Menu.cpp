#include "Scene_Menu.h"
#include "Scene_Play.h"

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W,			"UP");
	registerAction(sf::Keyboard::S,			"DOWN");
	registerAction(sf::Keyboard::D,			"PLAY");
	registerAction(sf::Keyboard::Escape,	"QUIT");

	m_entityManager = EntityManager();
	m_title = "menu";
	m_menuStrings.push_back("Level 1");
	m_menuStrings.push_back("Level 2");
	m_menuStrings.push_back("Level 3");

	m_levelPaths.push_back("level1.txt");
	m_levelPaths.push_back("level1.txt");
	m_levelPaths.push_back("level1.txt");

	m_menuText.setFont(m_game->assets().getFont("Hack"));
	m_menuText.setString(m_menuStrings[0]);
	m_menuText.setCharacterSize(72);
	m_menuText.setOrigin(sf::Vector2f(0, 36));

	//int x = m_game->window().getSize().x;
	////int y = m_game->window().getSize().y;

	//float pos = y / (m_menuStrings.size() + 1);

	m_menuText.setPosition(sf::Vector2f( 30.0, 0 ));

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

	// the below formula should evenly space the menu text entries
	// y = (1080 - 4 * 20) / 6 = 140

	float textY = (float)m_game->window().getSize().y / (float)m_menuStrings.size();
	float textYH = textY / 2.0f;
	m_menuText.setPosition(sf::Vector2f(50, textYH));

	for (auto s : m_menuStrings)
	{
		m_menuText.setString(s);
		m_game->window().draw(m_menuText);
		m_menuText.move(sf::Vector2f(0.0f, textY ));
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
			if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; std::cout << "up\n"; }
			else { m_menuIndex - m_menuStrings.size() - 1; std::cout << "up\n"; }
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
			std::cout << "down\n";
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
	m_game->quit();

}
