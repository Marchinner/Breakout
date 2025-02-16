#pragma once

#include <vector>

#include <Rendering/SpriteRenderer.h>
#include "GameObject.h"


class GameLevel
{
public:
    // Constructor
    GameLevel() {}

    // Load level from file
    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
    // Render level
    void Draw(SpriteRenderer& renderer);
    // Check if the level is completed
    bool IsCompleted();

public:
    // Level state
    std::vector<GameObject> Bricks;

private:
    // Initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData,
        unsigned int levelWidth, unsigned int levelHeight);
};

