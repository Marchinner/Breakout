#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <Core/GameObject.h>

struct Particle
{
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec4 Color;
    float Life;

    Particle()
        : Position{ 0.0f }, Velocity{ 0.0f }, Color{ 1.0f }, Life{ 0.0f } {
    }
};

class ParticleGenerator
{
public:
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    void Update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2{ 0.0f, 0.0f });
    void Draw();

private:
    void init();
    unsigned int firstUnusedParticle();
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2{0.0f, 0.0f} );

private:
    // State
    std::vector<Particle> particles;
    unsigned int amount;
    
    // Render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO{ 0 };
};

