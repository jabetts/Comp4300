#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "components.h"
#include "Action.h"

#include <iostream>
#include<memory>

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

    registerAction(sf::Keyboard::W,      "UP");
    registerAction(sf::Keyboard::A,      "LEFT");
    registerAction(sf::Keyboard::D,      "RIGHT");
    registerAction(sf::Keyboard::S,      "DOWN");
    registerAction(sf::Keyboard::Space,  "SHOOT");
    registerAction(sf::Keyboard::M,      "ANIM"); // animation debug

    // TODO: Register all other gameplay Actions

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Hack"));

    loadLevel(levelPath);
}

void Scene_Play::loadLevel(const std::string& filename)
{
    // TODO: read in the level file and add the appropriate entities
    //       use the PlayerConfig struct m_playerConfig to store player properties
    //       this struct is defined at the top of Scene_Play.h

    // NOTE: all of the code below is sample code which shows you how to
    //       set up and use entities with the new syntax, it should be removed

    sf::Clock clock;
    // rest the entity manager every time we load a level
    m_entityManager = EntityManager();

    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cerr << "Unable to open level file.\n";
        exit(1);
    }

    std::cout << "Loading level\n";
    std::string in;
    
    while (f >> in)
    {
        if (in == "Tile")
        {
            std::string text;
            int X, Y;
            f >> text >> X >> Y;
            auto t = m_entityManager.addEntity("Tile");
            t->addComponent<CAnimation>(m_game->assets().getAnimation(text), true);
            t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());
            t->addComponent<CTransform>(gridToMidPixel(X, Y, t));
        }
        else if (in == "Dec")
        {

        }
        else if (in == "Player")
        {
            f >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CW
                >> m_playerConfig.CH >> m_playerConfig.SPEED >> m_playerConfig.JUMP
                >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
        }
    }
    spawnPlayer();

    // some sample entities
    //
    // 55:44 in lecture
    //
    //auto e = m_entityManager.addEntity("tile");
    // IMPORTANT: always add the CAnimation componenet first so that gridToMidPixel can compute correctly
    //brick->addComponent<CAnimation>(m_game->assets().getAnimation("Ground"), true);
    //brick->addComponent<CTransform>(Vec2(96, 480));
    //brick->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());
    // NOTE: your final code should position the entitiy with the grid x,y position read from the file:
    //brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick);

    //if (brick->getComponent<CAnimation>().animation.getName() == "Ground")
    //{
    //    std::cout << "This could be a good way of identifing of a tile is a brick!\n";
    //}

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

    if (f.is_open()) { f.close(); }

    int time = clock.getElapsedTime().asMilliseconds();
    std::cout << "Level loaded in: " << time << " milliseconds\n";
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    // TODO: This function takes in a grid (x,y) position and an Entity
    //       return a Vec2 indicating where the CENTER position of the Entity should be
    //       You must use the Entity's Animation size to position it correctly
    //       The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    //       The bottom-left corner of the Animation should align with the bottom left of the grid cell
    
    auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
    int pixelX = (gridX * (float)m_gridSize.x) + size.x;
    int pixelY = height() - (gridY * m_gridSize.y) - size.y; // However we need to flip this as tile positions for y are reversed.


    return Vec2(pixelX, pixelY);
}

void Scene_Play::spawnPlayer()
{
    // here is a sample player entity which you can use to constrcut other entities
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanStand"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(1,1, m_player));
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CW, m_playerConfig.CH));
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CInput>();
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
    // TODO: Implement the maximum player speed in both the X and Y directions
    // NOTE: Setting an entity's scale.x to -1/1 will make it face left right

    //Vec2 playerVelocity(0, m_player->getComponent<CTransform>().velocity.y);
    //Vec2 playerVelocity(m_player->getComponent<CTransform>().velocity.x, 0);

    auto playerVelocity(m_player->getComponent<CTransform>().velocity);

    // player CInput
    auto& pInput = m_player->getComponent<CInput>();


    m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;

    if (pInput.up)
    {
        playerVelocity.y = -m_playerConfig.SPEED;
    }
    if (pInput.up == false)
    {
        playerVelocity.y = 0;
    }

    if (pInput.left)
    {
        playerVelocity.x = -m_playerConfig.SPEED;
    }
    else if (pInput.left == false)
    {
        playerVelocity.x = 0;
    }
    if (pInput.right) { playerVelocity.x = m_playerConfig.SPEED; }
    else if (!pInput.right == false)
    {
        playerVelocity.x = 0;
    }
    if (pInput.down) { playerVelocity.y = 3; }

    m_player->getComponent<CTransform>().velocity = playerVelocity;

    if (playerVelocity.x == 0 and playerVelocity.y == 0)
    {
        m_player->addComponent<CState>().state = "Stand";
    }

    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CGravity>())
        {
            // TODO: uncomment gravity when collisions work
            //e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
            // if the player is moving faster than max speed in any direction,
            // set that direction to the players max speed
        }

        //e->getComponent<CTransform>().velocity += e->getComponent<CTransform>().velocity;
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;

        if (e->getComponent<CTransform>().velocity.x < 0)
        {
            m_player->getComponent<CTransform>().scale.x = -1.0f;
        }
        if (e->getComponent<CTransform>().velocity.x > 0)
        {
            m_player->getComponent<CTransform>().scale.x = 1.0f;
        }
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

    // TODO: Dont like having to create an object just to do physics, so change the physics
    //       object to just the functions available through the .h
    Physics p;

    for (auto& e : m_entityManager.getEntities("Tile"))
    {   
        if (p.isCollision(e, m_player))
        {
            Vec2 overlap = p.GetOverlap(e, m_player);
            Vec2 pOverlap = p.GetPreviousOverlap(e, m_player);
            float h = (overlap.x > overlap.y) ? overlap.y : overlap.x;

            std::cout << " overlap: " << overlap.x << " " << overlap.y << std::endl;
            std::cout << "pOverlap: " << pOverlap.x << " " << pOverlap.y << std::endl;

            // Crude resolution
            Vec2 pPos = m_player->getComponent<CTransform>().pos;
            Vec2 ePos = e->getComponent<CTransform>().pos;

            // collision came from the left.
            if (pOverlap.y > 0 && pPos.x < ePos.x)
            {
                m_player->getComponent<CTransform>().pos.x -= overlap.x;
            }
            // collision came from the right.
            if (pOverlap.y > 0 && pPos.x > ePos.x)
            {
                m_player->getComponent<CTransform>().pos.x += overlap.x;
            }
            // collision came from the top
            if (pOverlap.x > 0 && pPos.y < ePos.y)
            {
                m_player->getComponent<CTransform>().pos.y -= overlap.y;
            }
            // collision came from the bottom
            if (pOverlap.x > 0 && pPos.y > ePos.y)
            {
                m_player->getComponent<CTransform>().pos.y += overlap.y;
            }
        }
     }
}

void Scene_Play::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; std::cout << "Grid " << (m_drawGrid ? "On\n" : "Off\n"); }
        else if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { onEnd(); }

        // player actions
        auto& comp = m_player->getComponent<CInput>();

        if (action.name() == "UP") 
        { 
            comp.up = true;
            m_player->addComponent<CState>().state = "Jump";
        }
        if (action.name() == "LEFT") 
        {
            m_player->getComponent<CInput>().left = true;  
            m_player->addComponent<CState>().state = "Run";
        }
        if (action.name() == "RIGHT") 
        {  
            m_player->getComponent<CInput>().right = true;
            m_player->addComponent<CState>().state = "Run";
        }
        if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
    }

    if (action.type() == "END")
    {
        if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
        if (action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = false;
        }
        if (action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = false;
        }
        if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; } 
    }
}

void Scene_Play::sAnimation()
{
    // TODO: Complete the Animation class first

    // Player animations
    auto& const comp = m_player->getComponent<CState>();
    auto& const currentAnim = m_player->getComponent<CAnimation>();

    if (comp.state == "Run")
    {
        if (currentAnim.animation.getName() != "MegamanRun")
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanRun"), true);
    }
    if (m_player->getComponent<CState>().state == "Stand")
    {
        if (currentAnim.animation.getName() != "MegamanStand")
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanStand"), true);
    }

    if (m_player->getComponent<CState>().state == "air")
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
    }

    // TODO: set the animation of the player based on its CState component
    // TODO: for each entity with an animation, call entitiy->getComponent<CAnimation>().animaiton.update()
    //       if the animation is not repeated, and it has ended, destroy the entity
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            e->getComponent<CAnimation>().animation.update();
        }
    }
   
}

void Scene_Play::onEnd()
{
    // reset the view
    float windowCenterX = m_game->window().getSize().x / 2.0f;
    sf::View view = m_game->window().getView();
    view.setCenter(m_game->window().getSize().x / 2.0f , m_game->window().getSize().y / 2.0f);
    m_game->window().setView(view);

    m_game->changeScene("menu", std::make_shared<Scene_Menu>(m_game), false);
}

void Scene_Play::sRender()
{
    m_game->window().clear();
    // 1:17:58 is lecture
    // color the background darker so you know that the game is paused
    if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
    else { m_game->window().clear(sf::Color(75, 75, 75)); }

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
      
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(sf::Vector2f(x, 0), sf::Vector2f(x, height()));

            for (float y = 0; y < height(); y += m_gridSize.y)
            {
                drawLine(sf::Vector2f(leftX, height() - y), sf::Vector2f(rightX, height() - y));
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
    m_game->window().display();
    m_currentFrame++;
}


void Scene_Play::sDebug()
{
}

void Scene_Play::sEnemySpawner()
{
}

void Scene_Play::drawLine(sf::Vector2f v1, sf::Vector2f v2)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = v1;
    line[1].position = v2;
    line[0].color = sf::Color::White;
    line[1].color = sf::Color::White;

    m_game->window().draw(line);
}

float Scene_Play::width() const
{
    return m_game->window().getSize().x;
}

float Scene_Play::height() const
{
    return m_game->window().getSize().y;
}




