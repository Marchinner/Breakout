#pragma once

#include "GameObject.h"

class BallObject :
    public GameObject
{
public:
    BallObject();
    BallObject(glm::vec2 position, float radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 Move(float deltaTime, unsigned int windowWidth);
    void Reset(glm::vec2 position, glm::vec2 velocity);

public:
    // Ball state
    float Radius;
    bool Stuck;
};

