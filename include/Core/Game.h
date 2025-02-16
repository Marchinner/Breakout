#pragma once

#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "GameLevel.h"

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE{ 100.0f, 20.0f };
// Initial velocity of the player paddle
const float PLAYER_VELOCITY{ 500.0f };

// Initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY{ 100.0f, -350.0f };
// Radius of the ball
const float BALL_RADIUS = 12.5f;

class Game
{
public:    
    // Constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();

    // Initialize game state
    void Init();

    // Game loop
    void ProcessInput(float deltaTime);
    void Update(float deltaTime);
    void Render();
    void DoCollisions();
    void ResetLevel();
    void ResetPlayer();

public:
    // Game state
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    std::vector<GameLevel> Levels;
    unsigned int Level{ 0 };
};

