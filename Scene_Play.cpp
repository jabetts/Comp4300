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
    registerAction(sf::Keyboard::P,      "COLLISIONS");         // Toggle collisions (P)
    registerAction(sf::Keyboard::O,      "DEBUG");              // Toggle debug text (O)

    registerAction(sf::Keyboard::W,      "UP");
    registerAction(sf::Keyboard::A,      "LEFT");
    registerAction(sf::Keyboard::D,      "RIGHT");
    registerAction(sf::Keyboard::S,      "DOWN");
    registerAction(sf::Keyboard::Space,  "SHOOT");
    

    // TODO: Register all other gameplay Actions

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Hack"));
    m_debugText.setFont(m_game->assets().getFont("Hack"));

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
    auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
    float pixelX = (gridX * (float)m_gridSize.x) + size.x;
    float pixelY = height() - (gridY * m_gridSize.y) - size.y; // However we need to flip this as tile positions for y are reversed.


    return Vec2(pixelX, pixelY);
}

void Scene_Play::spawnPlayer()
{
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanJump"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CW - 5, m_playerConfig.CH));
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

    float px = m_player->getComponent<CTransform>().velocity.x;
    float py = m_player->getComponent<CTransform>().velocity.y;

    std::string s = "vol x: " +std::to_string((float)px) + " vol y : " + std::to_string((float)py);
    m_debugText.setString(s);
    m_debugText.setFont(m_game->assets().getFont("Hack"));
    m_debugText.setCharacterSize(25);
    m_debugText.setPosition(m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y - 40);

    // player CInput
    auto& pInput = m_player->getComponent<CInput>();
    auto& pState = m_player->getComponent<CState>();

    m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;


    // TODO: Physics will change based on air or ground states
    if (pInput.up)
    {
        py += m_playerConfig.JUMP;
        pInput.canJump = false;
        pState.state = "Jump";
    }
    //if (pInput.up == false)
    //{
    //
    //}
    if (pInput.left)
    {
        if (px > 0)
            px -= 1.25;
        else
            px -= m_playerConfig.SPEED;
    }
    if (pInput.left == false)
    {
        // Decelerate
        if (px < 0)
        {
            px += 1.0;
        }
    }
    if (pInput.right) 
    { 
        if (px < 0)
            px += 1.25;
        else
            px += m_playerConfig.SPEED; 
    }
    if (pInput.right == false)
    {
        // Decelerate
        if (px > 0)
            px -= 1.0;
    }
    // The above login sometimes left a small drift in either direction when no inputs where current
    // This will zero off those small drifts
    if (pInput.right == false && pInput.left == false)
    {
        if (((px > -0.1 && px <= -0.5)  || (px > 0.1 && px <= 0.5)) && pState.state != "Jump")
            px = 0;
        //m_player->addComponent<CState>().state = "Stand";
    }

    // Max jump speed is MAXSPEED * 1.5
    if (std::abs(py) > (float)m_playerConfig.MAXSPEED * 1.5)
    {
        if (py < 0)
        py = (float) -m_playerConfig.MAXSPEED * 1.5;
    }

    // Max x speed direction
    if (std::abs(px) > m_playerConfig.MAXSPEED)
    {
        if (px < 0)
            px = -m_playerConfig.MAXSPEED;
        else
            px = m_playerConfig.MAXSPEED;
    }

    Vec2 playerVelocity(px, py);

    m_player->getComponent<CTransform>().velocity = playerVelocity;

    for (auto e : m_entityManager.getEntities())
    {
        // Do gravity first
        if (e->hasComponent<CGravity>())
        {
            e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
            if (e->getComponent<CTransform>().velocity.y > m_playerConfig.MAXSPEED * 3)
            //{
                // Terminal velocity
                e->getComponent<CTransform>().velocity.y = m_playerConfig.MAXSPEED * 3;
            //}
        }

        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;

        // Flip sprite based on movement direction
        if (e->getComponent<CTransform>().velocity.x < 0)
        {
            e->getComponent<CTransform>().scale.x = -1.0f;
        }
        if (e->getComponent<CTransform>().velocity.x > 0)
        {
            e->getComponent<CTransform>().scale.x = 1.0f;
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


    // TODO: Implement bullet / tile collisions
    //       Destroy the tile if it has a brick animation
    // TODO: Implement the player / tile collision and resolutions
    //       Update the CState componenet of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system
    // TODO: Check to see if the player has fallen down a hole (y > height())

    // TODO: Dont like having to create an object just to do physics, so change the physics
    //       object to just the functions available through the .h
    Physics p;

    // First test the player is not off the left side of the map or fallen off
    Vec2& pPos = m_player->getComponent<CTransform>().pos;

    if (pPos.x <= 0)
    {
        pPos.x = 0.0;
    }
    if (pPos.y > height())
    {
        pPos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
    }
    // TODO: make this so the player can go up to negative halfSize of the bounding box
    if (pPos.y <= 0)
    {
        pPos.y = 0;
    }

    // Check is player is falling, if so then the players state is jump
    auto& pState = m_player->getComponent<CState>();
    
    Vec2& pVel = m_player->getComponent<CTransform>().velocity;
    // Check for tile collisions

    bool collided = false;
    for (auto& e : m_entityManager.getEntities("Tile"))
    {   
        Vec2 pOverlap = p.GetPreviousOverlap(e, m_player);
        if (p.isCollision(e, m_player))
        {
            collided = true;
            Vec2 overlap = p.GetOverlap(e, m_player);
            Vec2 pOverlap = p.GetPreviousOverlap(e, m_player);
            float h = (overlap.x > overlap.y) ? overlap.y : overlap.x;

            // Crude collision resolution
            Vec2& pPos = m_player->getComponent<CTransform>().pos;
            Vec2& ePos = e->getComponent<CTransform>().pos;
            Vec2& pbox = m_player->getComponent<CBoundingBox>().size;
            Vec2& pVel = m_player->getComponent<CTransform>().velocity;

            //if (pState.state != "Ground")
            //    pState.state = "Jump";

            if (m_collisions)
            {
                // collision came from the top
                if (pOverlap.x > 0 && pPos.y < ePos.y)
                {
                    pPos.y -= overlap.y;
                    // change velocity to 0 if standing on a tile
                    pVel.y = 0;
                    // Set players state to can jump as they have landed
                    m_player->getComponent<CInput>().canJump = true;
                    
                    pState.state = "Ground";
                }
                // collision came from the bottom
                if (pOverlap.x > 0 && pPos.y > ePos.y)
                {
                    pPos.y += overlap.y;
                    // y velocity halves if hitting from below
                    pVel.y -= pVel.y / 2;

                }
                // collision came from the left.
                if (pOverlap.y > 0 && pPos.x < ePos.x)
                {
                    pPos.x -= overlap.x;
 
                    // stop x direction if hitting a tile
                    // TODO: If in the air do not 0 velocity by maybe half it..
                    pVel.x = 0;
                }
                // collision came from the right.
                if (pOverlap.y > 0 && pPos.x > ePos.x)
                {
                    pPos.x += overlap.x;
                    pVel.x = 0;
                }
            }
        }
        if (collided == false)
        {
            pState.state = "Jump";
            m_player->getComponent<CInput>().canJump = false;
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
        else if (action.name() == "COLLISIONS") { m_collisions = !m_collisions; }
        else if (action.name() == "DEBUG") { m_debugFlag = !m_debugFlag; }

        // player actions
        auto& pInput = m_player->getComponent<CInput>();
        auto& pState = m_player->getComponent<CState>().state;

        if (action.name() == "UP") 
        { 
            if (pInput.canJump)
            {
                pInput.up = true;
            }
        }
        if (action.name() == "LEFT") 
        {
            pInput.left = true;  

        }
        if (action.name() == "RIGHT") 
        {  
            pInput.right = true;
        }
        if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
        
    }

    if (action.type() == "END")
    {
        if (action.name() == "UP") 
        { 
            m_player->getComponent<CInput>().up = false; 
        }
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
    // Player animations
    auto& const pState = m_player->getComponent<CState>();
    auto& const currentAnim = m_player->getComponent<CAnimation>();
    auto& const pVel = m_player->getComponent<CTransform>().velocity;

    if (pState.state == "Ground")
    {
        if (pVel.x != 0)
        {
            if (currentAnim.animation.getName() != "MegamanRun")
                m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanRun"), true);
        }
        else
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanStand"), true);
        }
    }
    if (pState.state == "Jump")
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegamanJump"), true);
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
    // TODO: color the background darker so you know that the game is paused
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
                if (m_debugFlag)
                {
                    drawLine(sf::Vector2f(x, y), sf::Vector2f(x + m_gridSize.x, y + m_gridSize.y));
                    drawLine(sf::Vector2f(x + m_gridSize.x, y), sf::Vector2f(x, y + m_gridSize.y));
                }
                drawLine(sf::Vector2f(leftX, height() - y), sf::Vector2f(rightX, height() - y));
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }

    std::cout << m_player->getComponent<CState>().state << "\n";

    // DEBUG player pos
    if (m_debugFlag)
    {
        sf::CircleShape shape(3.0f, 6);
        shape.setPosition(sf::Vector2f(pPos.x, pPos.y));
        shape.setFillColor(sf::Color::Magenta);
        shape.setOrigin(shape.getRadius(), shape.getRadius());
        m_game->window().draw(shape);
        m_game->window().draw(shape);
        m_game->window().draw(m_debugText);
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




