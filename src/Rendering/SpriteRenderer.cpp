#include "Rendering/SpriteRenderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer(Shader& shader)
    : shader{ shader }
    , quadVAO{ 0 }
{
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    // Prepare transformations
    this->shader.Use();
    glm::mat4 model{ glm::mat4{1.0f} };
    model = glm::translate(model, glm::vec3{ position, 0.0f });

    model = glm::translate(model, glm::vec3{ 0.5f * size.x, 0.5f * size.y, 0.0f });
    model = glm::rotate(model, glm::radians(rotate), glm::vec3{ 0.0f, 0.0f, 1.0f });
    model = glm::translate(model, glm::vec3{ -0.5f * size.x, -0.5f * size.y, 0.0f });

    model = glm::scale(model, glm::vec3{ size, 1.0f });

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);

    // Activate and bind the texture
    texture.Bind();

    // Bind the vertex array to draw
    glBindVertexArray(this->quadVAO);
    // Draw the vertex array
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Unbind after draw
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
    // Configure VAO/VBO
    unsigned int VBO;

    // Define vertices
    float vertices[]{
        // position     // texture
        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 0.0f,     0.0f, 0.0f,

        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 1.0f,     1.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f
    };

    // Create and bind vertex array object
    glCreateVertexArrays(1, &this->quadVAO);

    // Create and bind vertex buffer object
    glCreateBuffers(1, &VBO);

    // Set VBO data
    glNamedBufferStorage(VBO, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(this->quadVAO, 0, VBO, 0, 4 * sizeof(float));

    // Enable VAO attributes
    glEnableVertexArrayAttrib(this->quadVAO, 0);

    // Set VAO attribute at location 0
    glVertexArrayAttribFormat(this->quadVAO, 0, 4, GL_FLOAT, GL_FALSE, 0);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
