#pragma once

#include <glm/glm.hpp>

#include <Rendering/Texture.h>
#include <Rendering/SpriteRenderer.h>

class GameObject
{
public:
    // Constructor
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3{ 1.0f },
        glm::vec2 velocity = glm::vec2{ 0.0f, 0.0f });

    virtual void Draw(SpriteRenderer& renderer);

public:
    // Object state
    glm::vec2 Position;
    glm::vec2 Size;
    glm::vec2 Velocity;
    glm::vec3 Color;
    float Rotation;
    bool IsSolid;
    bool Destroyed;

    // Render state
    Texture2D Sprite;
};

