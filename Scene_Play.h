#pragma once


#include <map>
#include <memory>
#include "Scene.h"
#include "EntityManager.h"


class Scene_Play : public Scene
{
	struct PlayerConfig
	{
		float X, Y, CW, CH, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct Intersect { bool result; Vec2 pos; float t; };

protected:

	std::shared_ptr<Entity>		m_player;
	std::string					m_levelPath;
	PlayerConfig				m_playerConfig;
	bool						m_drawTextures = true;
	bool						m_drawCollision = false;
	bool						m_drawGrid = false;
	bool						m_collisions = true;
	bool						m_debugFlag = false;
	bool						m_losFlag = false;
	bool						m_drawLOS = true;
	Vec2						m_gridSize = {64, 64};
	sf::Text					m_gridText;
	sf::Text                    m_debugText;
	std::string                 m_debugString;
	Vec2						m_mPos;
	sf::CircleShape             m_shape;

	void init(const std::string& levelPath);
	void loadLevel(const std::string& filename);

	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sCollision();
	void sLifeSpan();
	void sDebug();
	void sDrag();
	void sLOS();
	void sLos(); // improved version
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	Vec2 pixelToGrid(Vec2 pos);
	Vec2 windowToWorld(const Vec2& window) const;
	void calcLOS(std::shared_ptr<Entity> p, std::shared_ptr<Entity> e);
	Intersect lineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d);

	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void drawLine(sf::Vector2f v1, sf::Vector2f v2);


	float width() const;

	float height() const;

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	void sRender();
};

