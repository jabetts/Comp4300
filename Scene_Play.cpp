#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "components.h"
#include "Action.h"
#include "RayCast.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <array>

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
    registerAction(sf::Keyboard::P,      "PAUSE");              // Toggle pause (P)
    registerAction(sf::Keyboard::O,      "DEBUG");              // Toggle debug text (O)

    registerAction(sf::Keyboard::W,      "UP");
    registerAction(sf::Keyboard::A,      "LEFT");
    registerAction(sf::Keyboard::D,      "RIGHT");
    registerAction(sf::Keyboard::S,      "DOWN");
    registerAction(sf::Keyboard::Space,  "SHOOT");
    
    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Hack"));
    m_debugText.setFont(m_game->assets().getFont("Hack"));

    loadLevel(levelPath);
}

// If pos is inside the entity
bool IsInside(Vec2 pos, std::shared_ptr<Entity> e)
{
    auto ePos = e->getComponent<CTransform>().pos;
    auto size = e->getComponent<CAnimation>().animation.getSize();
    float dx = fabs(pos.x - ePos.x);
    float dy = fabs(pos.y - ePos.y);

    return (dx <= size.x / 2) && (dy <= size.y / 2);
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
    //std::string in;
    std::string line;
    
    while (std::getline(f, line))
    {
        // Skip comments
        if (line[0] == '#')
        {
            continue;
        }

        std::istringstream iss(line);
        std::string in;
        iss >> in;

        if (in == "Tile")
        {
            std::string text;
            float X, Y;
            iss >> text >> X >> Y;
            auto t = m_entityManager.addEntity("Tile");
            t->addComponent<CAnimation>(m_game->assets().getAnimation(text), true);
            t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());
            t->addComponent<CTransform>(gridToMidPixel(X, Y, t));
            t->addComponent<CDraggable>();
        }
        else if (in == "Dec")
        {
            std::string text;
            float X, Y;
            iss >> text >> X >> Y;
            auto t = m_entityManager.addEntity("Dec");
            t->addComponent<CAnimation>(m_game->assets().getAnimation(text), true);
            t->addComponent<CTransform>(gridToMidPixel(X, Y, t));

        }
        else if (in == "Player")
        {
            iss >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CW
                >> m_playerConfig.CH >> m_playerConfig.SPEED >> m_playerConfig.JUMP
                >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
        }
    }
    spawnPlayer();

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

Vec2 Scene_Play::pixelToGrid(Vec2 pos)
{
    float x = pos.x / (float)m_gridSize.x;
    float y = (height() - pos.y) / m_gridSize.y;
    return Vec2(x,y);
}

Vec2 Scene_Play::windowToWorld(const Vec2& window) const
{
    auto& view = m_game->window().getView();

    float wx = view.getCenter().x - m_game->window().getSize().x / 2;
    float wy = view.getCenter().y - m_game->window().getSize().y / 2;

    return Vec2(window.x + wx, window.y + wy);
}

void Scene_Play::calcLOS(std::shared_ptr<Entity> p, std::shared_ptr<Entity> e)
{
    auto pt = p->getComponent<CTransform>().pos;
    auto et = e->getComponent<CTransform>().pos;
    auto bx = e->getComponent<CBoundingBox>().halfSize.x;
    auto by = e->getComponent<CBoundingBox>().halfSize.y;
    
    // Top left
    drawLine({ pt.x, pt.y }, { et.x - bx, et.y - by });
    drawLine({ pt.x, pt.y }, { et.x + bx, et.y - by });
    drawLine({ pt.x, pt.y }, { et.x - bx, et.y + by });
    drawLine({ pt.x, pt.y }, { et.x + bx, et.y + by });
}

// a is ray origin, b is line target, c is line segment start, d is line segment end
Scene_Play::Intersect Scene_Play::lineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d)
{
    
    Vec2 r = (b - a);
    Vec2 s = (d - c);
    float rxs = r.crossProduct(s);
    Vec2 cma = c - a;
    float t = (cma.crossProduct(s)) / rxs;
    float u = (cma.crossProduct(r)) / rxs;

    if (t >=0 && t <= 1 && u >= 0 && u <=1)
        return { true, Vec2(a.x + t*r.x, a.y + t*r.y), t };
    else
        return { false, Vec2(0,0), t };
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
    // Only 2 bullets can be active at a time
    auto t = m_entityManager.getEntities("Bullet");
    if (t.size() > 1) return;

    auto b = m_entityManager.addEntity("Bullet");
    b->addComponent<CAnimation>(m_game->assets().getAnimation("Bullet"), true);
    b->addComponent<CLifeSpan>(30, m_currentFrame);
    b->addComponent<CTransform>().pos = entity->getComponent<CTransform>().pos;
    b->addComponent<CBoundingBox>(m_game->assets().getAnimation("Bullet").getSize());
    float direction = entity->getComponent<CAnimation>().animation.getSprite().getScale().x;
    b->getComponent<CTransform>().velocity.x = entity->getComponent<CTransform>().velocity.x + (15 * direction);
}

void Scene_Play::update()
{
    m_entityManager.update();
    if (!m_paused)
    {
        sMovement();
        sLifeSpan();
        sCollision();
        //sLOS();
        sAnimation();
        sDrag();
    }
    m_currentFrame++;
    sRender();
}

void Scene_Play::sMovement()
{
    float px = m_player->getComponent<CTransform>().velocity.x;
    float py = m_player->getComponent<CTransform>().velocity.y;
    std::string s("Blank");
    m_debugText.setString(s);
    m_debugText.setFont(m_game->assets().getFont("Hack"));
    m_debugText.setCharacterSize(25);
    m_debugText.setPosition(m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y - 40);

    auto& pInput = m_player->getComponent<CInput>();
    auto& pState = m_player->getComponent<CState>();

    m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;

    // Player movement physics.. this is very basic
    if (pInput.up)
    {
        if (pState.state != "Jump")
        {
            py += m_playerConfig.JUMP;
            pInput.canJump = false;
            pState.state = "Jump";
        }
    }
    if (pInput.up == false)
    {
        if (pState.state == "Jump")
        {
            if (py < 0)
            {
                py += 1.0;
            }
        }
    }
    if (pInput.left)
    {
        if (pState.state == "Ground")
        {
            if (px > 0)
                px -= 1.25;
            else
                px -= m_playerConfig.SPEED;
        }
        else
        {
            if (px > 0)
                px -= 0.25;
            else
                px -= m_playerConfig.SPEED / 2.5f;
        }
    }
    if (pInput.left == false)
    {
        // Decelerate
        if (pState.state == "Ground")
        {
            if (px < 0)
            {
                px += 1.0;
            }
        }
        // if jumping
        else
        {
            if (px < 0)
            {
                px += 0.5;
            }
        }
    }
    if (pInput.right)
    {
        if (pState.state == "Ground")
        {
            if (px < 0)
                px += 1.25;
            else
                px += m_playerConfig.SPEED;
        }
        else
        {
            if (px < 0)
                px += 0.25;
            else
                px += m_playerConfig.SPEED / 2.5f;
        }
    }
    if (pInput.right == false)
    {
        // Decelerate
        if (pState.state == "Ground")
        {
            if (px > 0)
                px -= 1.0;
        }
        else
        {
            if (px > 0)
                px -= 0.5;
        }
    }
    // This will zero off any small drifts lwft over after the above basic physics calcs
    if (pInput.right == false && pInput.left == false)
    {
        if ((std::abs(px) > 0.01f && std::abs(px) <= 0.9f))
        {
            px = 0;
        }
    }

    // Max jump speed is MAXSPEED * 3
    if (std::abs(py) > (float)m_playerConfig.MAXSPEED * 2)
    {
        if (py < 0)
        py = (float) -m_playerConfig.MAXSPEED * 2;
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
            {
                // Terminal velocity
                e->getComponent<CTransform>().velocity.y = m_playerConfig.MAXSPEED * 3;
            }
        }

        // Add the velocities to the entities
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
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CLifeSpan>())
        {
            e->getComponent<CLifeSpan>().lifespan--;
            if (e->getComponent<CLifeSpan>().lifespan <= 0)
            {
                e->destroy();
            }
        }
    }
}

void Scene_Play::sCollision()
{
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
    if (pPos.y <= 0 - m_player->getComponent<CAnimation>().animation.getSize().y)
    {
        pPos.y = 0 - m_player->getComponent<CAnimation>().animation.getSize().y;
    }

    auto& pState = m_player->getComponent<CState>();
    Vec2& pVel = m_player->getComponent<CTransform>().velocity;

    bool collided = false;
    
    // testing for collions with tiles only in this game
    for (auto& e : m_entityManager.getEntities("Tile"))
    {   
        auto& animation = e->getComponent<CAnimation>().animation;

        //if (animation.getName() == "Dec") continue;
        //if (animation.getName() == "Bullet") continue;

        Vec2 pOverlap = p.GetPreviousOverlap(e, m_player);

        // testing for bullet collisions first
        for (auto& b : m_entityManager.getEntities("Bullet"))
        {
            if (p.isPointCollision(e, b))
            {
                b->destroy();

                if (animation.getName() == "Brick" ||
                    animation.getName() == "BrickSpecial")
                {
                    e->removeComponent<CBoundingBox>();
                    e->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                }
            }
        }
        // Testing tile collision with the player
        if (p.isCollision(e, m_player))
        {
            collided = true;
            Vec2 overlap = p.GetOverlap(e, m_player);

            Vec2& pPos = m_player->getComponent<CTransform>().pos;
            Vec2& ePos = e->getComponent<CTransform>().pos;
            Vec2& pbox = m_player->getComponent<CBoundingBox>().size;
            Vec2& pVel = m_player->getComponent<CTransform>().velocity;

            if (m_collisions)
            {
                // collision came from the top
                if (pOverlap.x > 0 && pPos.y < ePos.y)
                {
                    pPos.y -= overlap.y;
                    pVel.y = 0;
                    m_player->getComponent<CInput>().canJump = true;
                    pState.state = "Ground";
                    // change velocity to 0 if standing on a tile
                    // Set players state to can jump as they have landed
                }
                // collision came from the bottom
                else if (pOverlap.x > 0 && pPos.y > ePos.y)
                {
                    pPos.y += overlap.y;
                    // y velocity halves if hitting from below
                    if (pVel.y != 0 ) pVel.y = pVel.y / 6;
                    if (e->isActive() && animation.getName() == "Brick")
                    {
                        if (pVel.y < 5)
                        {
                            e->destroy();
                        }
                        // brick smash animation
                        float angle = 180;
                        float step = 60;
                        for (int i = 0; i < 4; i++)
                        {
                            float rad = angle * (3.14159265 / 180.0);
                            Vec2 velocity(5 * cos(rad), 5 * sin(rad));
                            Vec2 scale(1.0, 1.0);
                            auto b = m_entityManager.addEntity("Dec");
                            b->addComponent<CAnimation>(m_game->assets().getAnimation("DecBrickSmall"), true);
                            b->addComponent<CLifeSpan>(15, m_currentFrame);
                            b->addComponent<CGravity>(0.15);
                            b->addComponent<CTransform>(e->getComponent<CTransform>().pos, velocity, scale, 0);
                            angle += step;
                        }
                    }
                    else if(animation.getName() == "QuestionFull")
                    {
                        e->addComponent<CAnimation>(m_game->assets().getAnimation("QuestionEmpty"), true);

                        // Spawn coin
                        Vec2 pos = e->getComponent<CTransform>().pos;
                        pos.y -= 64;
                        auto t = m_entityManager.addEntity("Dec");
                        t->addComponent<CAnimation>(m_game->assets().getAnimation("DecCoin"), true);
                        t->addComponent<CLifeSpan>(30, m_currentFrame);
                        t->addComponent<CTransform>(pos);
                        t->getComponent<CTransform>().velocity.y = -6.0f;
                    }
                    else if (animation.getName() == "BrickSpecial")
                    {
                        e->removeComponent<CBoundingBox>();
                        e->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                    }
                }
                // collision came from the left.
                else if (pOverlap.y > 0 && pPos.x < ePos.x)
                {
                    pPos.x -= overlap.x + 1;
                    // stop x direction if hitting a tile
                    pVel.x = 0;
                }
                // collision came from the right.
                else if (pOverlap.y > 0 && pPos.x > ePos.x)
                {
                    pPos.x += overlap.x + 1;
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
        // non game play actions
        if      (action.name() == "TOGGLE_TEXTURE")   { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID")      { m_drawGrid = !m_drawGrid; std::cout << "Grid " << (m_drawGrid ? "On\n" : "Off\n"); }
        else if (action.name() == "PAUSE")            { setPaused(!m_paused); }
        else if (action.name() == "QUIT")             { onEnd(); }
        else if (action.name() == "COLLISIONS")       { m_collisions = !m_collisions; }
        else if (action.name() == "DEBUG")            { m_debugFlag = !m_debugFlag; }

        // mouse actions
        else if (action.name() == "LEFT_CLICK")
        {
            Vec2 worldPos = windowToWorld(action.pos());
            
            for (auto e : m_entityManager.getEntities())
            {
                if (e->hasComponent<CDraggable>() && IsInside(worldPos, e))
                {
                    std::cout << e->getComponent<CAnimation>().animation.getName() << std::endl;
                    e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
                }
            }
        }
        else if (action.name() == "RIGHT_CLICK")
        {
       
        }
        else if (action.name() == "MIDDLE_CLICK")
        {
   
        }
        else if (action.name() == "MOUSE_MOVE")
        {
            m_mPos = action.pos();
            Vec2 worldPos = windowToWorld(m_mPos);
        }

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

        if (action.name() == "PAUSE") { m_paused = !m_paused; }

        if (action.name() == "SHOOT") 
        {
            m_player->getComponent<CInput>().canShoot = false;
            spawnBullet(m_player);
        }
        
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

    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            e->getComponent<CAnimation>().animation.update();
            if (!e->getComponent<CAnimation>().repeat)
            {
                if (e->getComponent<CAnimation>().animation.hasEnded())
                {
                    e->destroy();
                }
            }
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
    if (!m_paused) { m_game->window().clear(sf::Color(148, 148, 255)); }
    else { m_game->window().clear(sf::Color(148, 148, 255)); }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto& pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);
    sf::View ui = m_game->window().getDefaultView();

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

    sLos();

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
                std::string yCell = std::to_string((int)(height() - ((int)y) - 1) / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }

    if (m_losFlag)
    {
        // Draw LOS debug information
    }
    
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

    // Draw UI stuff
    m_game->window().setView(ui);
    Vec2 playerGrid(m_player->getComponent<CTransform>().pos);
    playerGrid = pixelToGrid(playerGrid);
    std::stringstream ss;
    ss << "grid x: " << (int)playerGrid.x << " " << "grid y: " << (int)playerGrid.y << "\n" <<
        "pos x:" << m_player->getComponent<CTransform>().pos.x << "\n";
    m_debugString = ss.str();
    m_debugText.setString(m_debugString);
    m_debugText.setPosition(10, 5);
    m_debugText.setCharacterSize(30);

    m_game->window().draw(m_debugText);


    m_game->window().display();
    m_currentFrame++;
}

void Scene_Play::sDebug()
{
}

void Scene_Play::sDrag()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
        {
            Vec2 worldPos = windowToWorld(m_mPos);
            e->getComponent<CTransform>().pos = worldPos;
            std::cout << "Dragging: " << e->getComponent<CAnimation>().animation.getName() 
                << " [" << m_mPos.x << "][" << m_mPos.y <<"]" << std::endl;
        }
    }
}

// TODO: Shoot a ray 0.00001 radians to each side of the rays being cae at each entity point
//       we can later use this to get true raycasted line of sight and use it to draw shadows
//       in the non seen areas.
std::array<Vec2, 4> getTileCorners(std::array<Vec2, 4> a, const sf::FloatRect& rect)
{
    a[0] = { rect.left, rect.top };
    a[1] = { rect.left, rect.top + rect.height };
    a[2] = { rect.left + rect.width, rect.top + rect.height };
    a[3] = { rect.left + rect.width, rect.top };

    return a;
}

void Scene_Play::sLos()
{
    Vec2 playerPos = m_player->getComponent<CTransform>().pos;
    Vec2 rayTarget(0, 0);
    Vec2 playerTilePos;
    float DistanceLimit = width() - (width() / 8);
    sf::CircleShape shape(6.f, 8);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(6.f, 6.f);

    sf::View view = m_game->window().getView();
    sf::Vector2f viewcenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    Vec2 viewPos(viewcenter.x - (viewSize.x / 2), viewcenter.y - (viewSize.y / 2));

    // First loop through all entities that we want to fire rays at each point
    for (auto& e : m_entityManager.getEntities("Tile"))
    {
        // first test if the tile is outside our bounds limit
        // using the players, and tiles grid position.
        Vec2 playerPos = m_player->getComponent<CTransform>().pos;
        Vec2 entityPos = e->getComponent<CTransform>().pos;
        float distance = playerPos.dist(entityPos);
        constexpr size_t SIGHT_LIMIT = 1000;
        
        int intersectDist = 1000000;
        
        if (distance < SIGHT_LIMIT) // TODO: Change this to the DistanceLimit
        {
            // Shoot a ray at each of the tiles points, then loop through
            // all entities within the Distance limit and find the closest
            // insterect for each ray

            Vec2 rays[4] = { };
            sf::FloatRect rect = e->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();

            rays[0] = { rect.left, rect.top }; // Top left
            rays[1] = { rect.left, rect.top + rect.height }; // Bottom left
            rays[2] = { rect.left + rect.width, rect.top + rect.height }; // bottom right
            rays[3] = { rect.left + rect.width, rect.top }; // top right

            // Check intersections for each ray for entities within the bounds
            int lowest = 2;
            
            for (size_t i = 0; i < 4; i++)
            {
                Vec2 closestPoint = rays[i];

                intersectDist = SIGHT_LIMIT + 1;
                for (auto& e : m_entityManager.getEntities("Tile"))
                {
                    Vec2 entityPos = e->getComponent<CTransform>().pos;
                    float distance = playerPos.dist(entityPos);

                    if (distance < SIGHT_LIMIT)
                    {
                        // TODO: Shoot a ray slightly to each side of each entities point.
                        sf::FloatRect r = e->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
                        std::array<Vec2, 4> corners;
                        corners = getTileCorners(corners, r);

                        Intersect lines[4];
                        lines[0] = lineIntersect(playerPos, rays[i], {r.left, r.top}, {r.left + r.width, r.top});
                        lines[1] = lineIntersect(playerPos, rays[i], corners[0], {r.left, r.top + r.height});
                        lines[2] = lineIntersect(playerPos, rays[i], {r.left, r.top + r.height}, {r.left + r.width, r.top + r.height});
                        lines[3] = lineIntersect(playerPos, rays[i], {r.left + r.width, r.top}, {r.left + r.width, r.top + r.height});

                        // work out which intersect is closest to the player if it intersects
                        float intersect = 2.0;

                        if (lines[0].result || lines[1].result || lines[2].result || lines[3].result)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                if (lines[i].result && std::abs(lines[i].t) < intersect)
                                {
                                    intersect = lines[i].t;
                                    lowest = i;
                                }
                            }
                        }
                        else
                        {
                            continue;
                        }
                        float tempDist = std::abs(playerPos.dist(lines[lowest].pos));
                        if (tempDist < intersectDist)
                        {
                            intersectDist = tempDist;
                            closestPoint = lines[lowest].pos;
                        }
                    }
                }
                if (m_drawCollision) 
                {
                    drawLine({ playerPos.x, playerPos.y }, { closestPoint.x, closestPoint.y });
                    shape.setPosition(sf::Vector2f(closestPoint.x, closestPoint.y));
                    m_game->window().draw(shape);
                }
            }   
        }
    }
}

// This L.O.S system fires rays radially in all directions up to n distance.
void Scene_Play::sRadialLOS()
{
    auto pos = m_player->getComponent<CTransform>().pos;
    Vec2 rpos(0, 0); // ray destination

    float distance = 10000000;
    Intersect dist;
    
    sf::CircleShape shape(6.f, 8);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(6.f, 6.f);

    float angle = 180;
    float step = 360 / 100;
    float rads = 0;
    int lowest = -1;
    int length = 1000;

    // lets start by casting 50 rays
    for (size_t i = 0; i < 120; i++)
    {
        rads = angle * 3.14f / 180.0f;
        rpos.x = pos.x + length * std::cos(rads);
        rpos.y = pos.y + length * std::sin(rads);
        Vec2 closestPoint = rpos;
        distance = 10000000;

        // For each entity, check the ray against each edge.
        for (auto& e : m_entityManager.getEntities("Tile"))
        {
            sf::FloatRect r = e->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();

            // each edge of the entity
            Intersect lines[4];
            lines[0] = lineIntersect({ pos.x, pos.y }, rpos, { r.left, r.top }, { r.left + r.width, r.top });
            lines[1] = lineIntersect({ pos.x, pos.y }, rpos, { r.left, r.top }, { r.left, r.top + r.height });
            lines[2] = lineIntersect({ pos.x, pos.y }, rpos, { r.left, r.top + r.height }, { r.left + r.width, r.top + r.height });
            lines[3] = lineIntersect({ pos.x, pos.y }, rpos, { r.left + r.width, r.top }, { r.left + r.width, r.top + r.height });

            // work out which intersect is closest to the player if it intersects
            float intersect = 2.0;
            
            if (lines[0].result || lines[1].result || lines[2].result || lines[3].result)
            {
                for (int i = 0; i < 4; i++)
                {
                    if (lines[i].result && std::abs(lines[i].t) < intersect)
                    {
                        intersect = lines[i].t;
                        lowest = i;
                    }
                }
            }
            else
            {
                continue;
            }
            float tempDist = std::abs(pos.distSq(lines[lowest].pos));
            if (tempDist < distance)
            {
                distance = tempDist;
                closestPoint = lines[lowest].pos;
            }
        }
        angle += step;

        if (m_drawCollision)
        {
            drawLine({ pos.x, pos.y }, { closestPoint.x, closestPoint.y });
            shape.setPosition(sf::Vector2f(closestPoint.x, closestPoint.y));
            m_game->window().draw(shape);
        }
    }
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
