#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

class SpriteRenderer
{
public:
    SpriteRenderer(Shader& shader);
    ~SpriteRenderer();

    void DrawSprite(Texture2D& texture, glm::vec2 position,
        glm::vec2 size = glm::vec2{ 10, 10 },
        float rotate = 0.0f,
        glm::vec3 color = glm::vec3{ 1.0f });

private:
    void initRenderData();

private:
    Shader shader;
    unsigned int quadVAO;
};

