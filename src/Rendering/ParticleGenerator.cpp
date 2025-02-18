#include "Rendering/ParticleGenerator.h"

#include <glad/glad.h>

// Stores the index of the last particle used
unsigned int lastUsedParticle{ 0 };

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : amount{ amount }
    , shader{ shader }
    , texture{ texture }
{
    init();
}

void ParticleGenerator::Update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
    // Add new particles
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(this->particles[unusedParticle], object, offset);
    }

    // Update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle& p = this->particles[i];
        p.Life -= deltaTime;
        if (p.Life > 0.0f)
        {
            p.Position -= p.Velocity * deltaTime;
            p.Color.a -= deltaTime * 2.5f;
        }
    }
}

void ParticleGenerator::Draw()
{
    // Use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();

    for (const Particle& particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
                
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }

    // Dont forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // Set meseh and attribute properties
    unsigned int VBO;
    float particle_quad[]{
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    // DSA buffer creation
    glCreateBuffers(1, &VBO);
    // Allocate immutable storage and initialize data.
    glNamedBufferStorage(VBO, sizeof(particle_quad), particle_quad, GL_DYNAMIC_STORAGE_BIT);

    // DSA VAO creation and configuration
    glCreateVertexArrays(1, &this->VAO);
    // Bind the VBO to the VAO at binding index 0
    glVertexArrayVertexBuffer(this->VAO, 0, VBO, 0, sizeof(float) * 4);

    // Setup vertex attribute 0: position (2 floats, starting at offset 0)
    glEnableVertexArrayAttrib(this->VAO, 0);
    glVertexArrayAttribFormat(this->VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(this->VAO, 0, 0);

    // Setup vertex attribute 1: color (2 floats, starting at offset 2*sizeof(float))
    glEnableVertexArrayAttrib(this->VAO, 1);
    glVertexArrayAttribFormat(this->VAO, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
    glVertexArrayAttribBinding(this->VAO, 1, 0);

    // Create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        this->particles.push_back(Particle());
    }
}

unsigned int ParticleGenerator::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i)
    {
        if (this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    // Otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if (this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    lastUsedParticle = 0;

    return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);

    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4{ rColor, rColor, rColor, 1.0f };
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}
