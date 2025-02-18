#include "Core/Game.h"

#include<glm/gtc/matrix_transform.hpp>

#include <Core/ResourceManager.h>
#include <Core/BallObject.h>
#include <Rendering/SpriteRenderer.h>
#include <Rendering/ParticleGenerator.h>
#include <Rendering/PostProcessor.h>

// Game-related State data
SpriteRenderer* Renderer;
// Particles
ParticleGenerator* Particles;
// Player game objec
GameObject* Player;
// Ball game object
BallObject* Ball;
// Postprocessing
PostProcessor* Effects;

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
    : State(GameState::GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
}

void Game::Init()
{
    // Load shaders
    ResourceManager::LoadShader("assets/shaders/default.vert", "assets/shaders/default.frag", nullptr, "sprite");
    ResourceManager::LoadShader("assets/shaders/particle.vert", "assets/shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("assets/shaders/postprocess.vert", "assets/shaders/postprocess.frag", nullptr, "postprocess");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // Set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // Load textures
    ResourceManager::LoadTexture("assets/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("assets/textures/particle.png", true, "particle");
    ResourceManager::LoadTexture("assets/textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("assets/textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("assets/textures/block.png", false, "block");
    ResourceManager::LoadTexture("assets/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("assets/textures/powerup_chaos.png", true, "powerup_chaos");
    ResourceManager::LoadTexture("assets/textures/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::LoadTexture("assets/textures/powerup_increase.png", true, "poweerup_increase");
    ResourceManager::LoadTexture("assets/textures/powerup_passthrough.png", true, "powerup_passthrough");
    ResourceManager::LoadTexture("assets/textures/powerup_speed.png", true, "powerup_speed");
    ResourceManager::LoadTexture("assets/textures/powerup_sticky.png", true, "powerup_sticky");

    // Load levels
    GameLevel one; one.Load("assets/levels/one.level", this->Width, this->Height / 2);
    GameLevel two; two.Load("assets/levels/two.level", this->Width, this->Height / 2);
    GameLevel three; three.Load("assets/levels/three.level", this->Width, this->Height / 2);
    GameLevel four; four.Load("assets/levels/four.level", this->Width, this->Height / 2);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);

    // Player
    glm::vec2 playerPosition{ glm::vec2{
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y} };
    Player = new GameObject(playerPosition, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // Ball
    glm::vec2 ballPosition{ playerPosition + glm::vec2{PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f} };
    Ball = new BallObject(ballPosition, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    // Particles
    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500
    );

    // Effects
    Effects = new PostProcessor(
        ResourceManager::GetShader("postprocess"),
        this->Width,
        this->Height
    );
}

void Game::ProcessInput(float deltaTime)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * deltaTime;
        // move player
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;

                if (Ball->Stuck)
                {
                    Ball->Position.x -= velocity;
                }
            }
        }

        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;

                if (Ball->Stuck)
                {
                    Ball->Position.x += velocity;
                }
            }
        }

        if (this->Keys[GLFW_KEY_SPACE])
        {
            Ball->Stuck = false;
        }
    }
}

void Game::Update(float deltaTime)
{
    // Update objects
    Ball->Move(deltaTime, this->Width);

    // Check for collisions
    DoCollisions();

    if (Ball->Position.y >= this->Height)
    {
        this->ResetLevel();
        this->ResetPlayer();
    }

    // Update particles
    Particles->Update(deltaTime, *Ball, 2, glm::vec2{ Ball->Radius / 2.0f });

    // Update powerups
    this->UpdatePowerUps(deltaTime);

    if (ShakeTime > 0.0f)
    {
        ShakeTime -= deltaTime;
        if (ShakeTime <= 0.0f)
        {
            Effects->Shake = false;
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        Effects->BeginRender();
        // Draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
            glm::vec2{ 0.0f, 0.0f }, glm::vec2{ this->Width, this->Height }, 0.0f);

        this->Levels[this->Level].Draw(*Renderer);

        Player->Draw(*Renderer);
        Particles->Draw();
        Ball->Draw(*Renderer);
        Effects->EndRender();
        Effects->Render(glfwGetTime());

        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.Destroyed)
            {
                powerUp.Draw(*Renderer);
            }
        }
    }
}

// PowerUps
void ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        Player->Color = glm::vec3{ 1.0f, 0.5f, 1.0f };
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3{ 1.0f, 0.5f, 0.5f };
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Effects->Chaos)
        {
            Effects->Confuse = true;
        }
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->Confuse)
        {
            Effects->Chaos = true;
        }
    }
}

// Collision detection
bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);

            // If collision is true in the tuple
            if (std::get<0>(collision))
            {
                // Destroy block if not solid
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                    this->SpawnPowerUps(box);
                }
                else
                {
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                }

                // Collision resolution
                Direction direction = std::get<1>(collision);
                glm::vec2 differenceVector = std::get<2>(collision);

                if (!(Ball->PassThrough && !box.IsSolid))
                {
                    // Horizontal collision
                    if (direction == LEFT || direction == RIGHT)
                    {
                        // Reverse horizontal velocity
                        Ball->Velocity.x = -Ball->Velocity.x;
                        // relocate
                        float penetration = Ball->Radius - std::abs(differenceVector.x);
                        if (direction == LEFT)
                        {
                            Ball->Position.x += penetration;
                        }
                        else
                        {
                            Ball->Position.x -= penetration;
                        }
                    }
                    else // Vertical collision
                    {
                        Ball->Velocity.y = -Ball->Velocity.y; // Reverse vertical velocity
                        // relocate
                        float penetration = Ball->Radius - std::abs(differenceVector.y);
                        if (direction == UP)
                        {
                            Ball->Position.y -= penetration; // move ball back up
                        }
                        else
                        {
                            Ball->Position.y += penetration; // move ball back down
                        }
                    }
                }
            }
        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the paddle, and change velocity based on where it hit the paddle
        float centerBoard{ Player->Position.x + Player->Size.x / 2.0f };
        float distance{ (Ball->Position.x + Ball->Radius) - centerBoard };
        float percentage{ distance / (Player->Size.x / 2.0f) };

        // then move accordingly
        float strength{ 2.0f };
        glm::vec2 oldVelocity{ Ball->Velocity };
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
        Ball->Stuck = Ball->Sticky;
    }

    for (PowerUp& powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            if (powerUp.Position.y >= this->Height)
            {
                powerUp.Destroyed = true;
            }

            if (CheckCollision(*Player, powerUp))
            {
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
                powerUp.Activated = true;
            }
        }
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("assets/levels/one.level", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("assets/levels/two.level", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("assets/levels/three.level", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("assets/levels/four.level", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2{ this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y };
    Ball->Reset(Player->Position + glm::vec2{ PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f) }, INITIAL_BALL_VELOCITY);
}

bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(75)) // 1 in 75
    {
        this->PowerUps.push_back(
            PowerUp("speed", glm::vec3{ 0.5f, 0.5f, 1.0f }, 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed"))
        );
    }

    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(
            PowerUp("sticky", glm::vec3{ 1.0f, 0.5f, 1.0f }, 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky"))
        );
    }

    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(
            PowerUp("pass-throught", glm::vec3{ 0.5f, 1.5f, 1.0f }, 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough"))
        );
    }

    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(
            PowerUp("pad-size-increase", glm::vec3{ 1.0f, 0.6f, 0.4f }, 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase"))
        );
    }

    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(
            PowerUp("confuse", glm::vec3{ 1.0f, 0.3f, 0.3f }, 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse"))
        );
    }

    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(
            PowerUp("chaos", glm::vec3{ 0.9f, 0.25f, 0.25f }, 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos"))
        );
    }
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, const std::string& type)
{
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.Activated)
        {
            if (powerUp.Type == type)
            {
                return true;
            }
        }
    }

    return false;
}

void Game::UpdatePowerUps(float deltaTime)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * deltaTime;

        if (powerUp.Activated)
        {
            powerUp.Duration -= deltaTime;

            if (powerUp.Duration <= 0.0f)
            {
                // remove powerup from list (will later be removed
                powerUp.Activated = false;
                // deactivate effects
                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {
                        Ball->Sticky = false;
                        Player->Color = glm::vec3{ 1.0f };
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {
                        Ball->PassThrough = false;
                        Ball->Color = glm::vec3{ 1.0f };
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {
                        Effects->Confuse = false;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {
                        Effects->Chaos = false;
                    }
                }
            }
        }
    }

    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(), [](const PowerUp& powerUp) {return powerUp.Destroyed && !powerUp.Activated; }), this->PowerUps.end());
}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject& ball, GameObject& other)
{
    // Get center point of the circle first
    glm::vec2 center{ ball.Position + ball.Radius };

    // Calculate AABB info
    glm::vec2 aabbHalfExtents{ other.Size.x / 2.0f, other.Size.y / 2.0f };
    glm::vec2 aabbCenter{ other.Position.x + aabbHalfExtents.x, other.Position.y + aabbHalfExtents.y };

    // Get the difference vector between both centers
    glm::vec2 difference{ center - aabbCenter };
    glm::vec2 clamped{ glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents) };

    // Add clamped value to AABCenter and we get the value of box closest to circle
    glm::vec2 closest{ aabbCenter + clamped };

    // Retrieve vector between center circle and closest point AABB and check if lenght <= radius
    difference = closest - center;

    if (glm::length(difference) <= ball.Radius)
    {
        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    else
    {
        return std::make_tuple(false, UP, glm::vec2{ 0.0f, 0.0f });
    }
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}