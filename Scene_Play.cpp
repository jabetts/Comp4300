#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "components.h"
#include "Action.h"

#include <iostream>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
    registerAction(sf::Keyboard::P,      "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T,      "TOGGLE_TEXTURE");     // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C,      "TOGGLE_COLLISION");   // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G,      "TOGGLE_GRID");        // Toggle drawing (G)rid

    registerAction(sf::Keyboard::W,      "JUMP");

    // TODO: Register all other gameplay Actions

    m_gridText.setCharacterSize(12);
    //m_gridText.setFont();

    loadLevel(levelPath);
}

void Scene_Play::loadLevel(const std::string& filename)
{
    // rest the entity manager every time we load a level
    m_entityManager = EntityManager();

    // TODO: read in the level file and add the appropriate entities
    //       use the PlayerConfig struct m_playerConfig to store player properties
    //       this struct is defined at the top of Scene_Play.h

    // NOTE: all of the code below is sample code which shows you how to
    //       set up and use entities with the new syntax, it should be removed

    spawnPlayer();

    // some sample entities
    //
    // 55:44 in lecture
    //
    auto brick = m_entityManager.addEntity("tile");
    // IMPORTANT: always add the CAnimation componenet first so that gridToMidPixel can compute correctly
    brick->addComponent<CAnimation>(m_game->assets().getAnimation("KnightRun"), true);
    brick->addComponent<CTransform>(Vec2(96, 480));
    // NOTE: your final code should position the entitiy with the grid x,y position read from the file:
    //brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick);

    if (brick->getComponent<CAnimation>().animation.getName() == "KnightRun")
    {
        std::cout << "This could be a good way of identifing of a tile is a brick!\n";
    }

    auto block = m_entityManager.addEntity("tile");
    block->addComponent<CAnimation>(m_game->assets().getAnimation("KnightRun"), true);
    block->addComponent<CTransform>(Vec2(224, 480));
    // add a bounding box, this will now show up if we press the 'C' key
    block->addComponent<CBoundingBox>(m_game->assets().getAnimation("KnightRun").getSize());
    block->addComponent<CTransform>(Vec2(352, 480));

    // NOTE: THIS IS INCREDIBLY IMPORTANT PLEASE READ THIS EXAMPLE
    //       Components are now returned as references rather than pointers
    //       If you do not specify a reference variable type, it will COPY the component
    //       Here is an example:
    //
    //       This will COPY the transform into the variable 'transform1' - it is INCORRECT
    //       Any changes you make to transform1 will not be changed inside the entity
    //       auto transform1 = entity->get<CTransform>()
    //
    //       This will REFERENCE the transform with the variable 'transform2' - it is CORRECT
    //       Now any changes you make to transform2 will be changed inside the entitiy
    //       auto& transform2 = entity->get-><CTransform>()
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    // TODO: This function takes in a grid (x,y) position and an Entity
    //       return a Vec2 indicating where the CENTER position of the Entity should be
    //       You must use the Entity's Animation size to position it correctly
    //       The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    //       The bottom-left corner of the Animation should align with the bottom left of the grid cell

    return Vec2(0,0);
}

void Scene_Play::spawnPlayer()
{
    // here is a sample player entity which you can use to constrcut other entities
    m_player = m_entityManager.addEntity("Player");
    //m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    //m_player->addComponent<CTransform>(Vec2(224,352));
    //m_player->addComponent<CBoundingBox>(Vec2(48,48));

    // TODO: be sure to add the remaining componenets to the player
    //m_player->addComponent<CGravity>(0.1);
    m_player->getComponent<CAnimation>();

}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    // TODO: This should spawn a bullet at the given entity, going in the direction the entity is facing
}

void Scene_Play::update()
{
    m_entityManager.update();

    // TODO: implement pause functionality

    sMovement();
    sLifeSpan();
    sCollision();
    sAnimation();
    sRender();
}

void Scene_Play::sMovement()
{
    // TODO: Implement player movement / jumping based on its CInput componenet
    // TODO: Implement gravity's effect on the player
    // TODO: Implement the maximum player speen in both the X and Y directions
    // NOTE: Setting an entity's scake.x to -1/1 will make it face left right

    Vec2 playerVelocity(0, m_player->getComponent<CTransform>().velocity.y);

    if (m_player->getComponent<CInput>().up)
    {
        playerVelocity.y = -3;
    }

    m_player->getComponent<CTransform>().velocity = playerVelocity;

    for (auto e : m_entityManager.getEntities())
    {
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;

        // if the player is moving faster than max speed in any direction,
        // set that direction to the players max speed.
    }
}

void Scene_Play::sLifeSpan()
{
    // TODO: Check lifespan of entities that have them, and destroy them if they go over
}

void Scene_Play::sCollision()
{
    // REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
    //           This means jumping will have a negative y-component
    //           and gravity will have a positive y-component
    //           Also, something BELOW something else will have a y value GREATER than it

    // TODO: Implement Physics::GetOverlap() function, use it in this function

    // TODO: Implement bullet / tile collisions
    //       Destroy the tile if it has a brick animation
    // TODO: Implement the player / tile collision and resolutions
    //       Update the CState compinenet of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system
    // TODO: Check to see if the player has fallen down a hole (y > height())
    // TODO: Don't let the player walk off the left side of the map
}

void Scene_Play::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
             if (action.name() == "TOGGLE_TEXTURE")   { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID")      { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "PAUSE")            { setPaused(!m_paused); }
        else if (action.name() == "QUIT")             { onEnd(); }
        else if (action.name() == "UP")
             {
                 m_player->getComponent<CInput>().up = true;
             }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP")
        {
            m_player->getComponent<CInput>().up = false;
        }
    }
}

void Scene_Play::sAnimation()
{
    // TODO: Complete the Animation class first

    if (m_player->getComponent<CState>().state == "air")
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
    }

    // TODO: set the animation of the player based on its CState component
    // TODO: for each entity with an animation, call entitiy->getComponent<CAnimation>().animaiton.update()
    //       if the animation is not repeated, and it has ended, destroy the entity
}

void Scene_Play::onEnd()
{
    // TODO: When the scene ends, change back to the MENU scene
    //       use m_game->changeScene(correct params);
}

void Scene_Play::sRender()
{
    m_game->window().clear();
    // 1:17:58 is lecture
    // color the background darker so you know that the game is paused
    if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
    else { m_game->window().clear(sf::Color(50, 50, 150)); }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto& pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);

    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game->window().draw(animation.getSprite());

            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that students can easily debug
    if (m_drawGrid)
    {
        /*
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX + width() + m_gridSize.x;

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
        */
    }
    
    m_game->window().display();
}


void Scene_Play::sDebug()
{
}

void Scene_Play::sEnemySpawner()
{
}

void Scene_Play::drawLine(Vec2 vec1, Vec2 vec2)
{

}




