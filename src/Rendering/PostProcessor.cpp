#include "Rendering/PostProcessor.h"

#include <glad/glad.h>

#include <iostream>
PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned height)
    : PostProcessingShader{ shader }
    , Texture{}
    , Width{ width }
    , Height{ height }
    , Confuse{ false }
    , Chaos{ false }
    , Shake{ false }
{
    // Initialize renderbuffer/framebuffer object
    glCreateFramebuffers(1, &this->MSFBO);
    glCreateRenderbuffers(1, &this->RBO);

    // Initialize renderbuffer storage with a multisampled color buffer
    glNamedRenderbufferStorageMultisample(this->RBO, 4, GL_RGB8, width, height);
    glNamedFramebufferRenderbuffer(this->MSFBO, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);
    // Verify that the framebuffer is complete
    if (glCheckNamedFramebufferStatus(this->MSFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
    }

    // Initialize FBO/texture
    glCreateFramebuffers(1, &this->FBO);
    this->Texture.Generate(width, height, nullptr);
    glNamedFramebufferTexture(this->FBO, GL_COLOR_ATTACHMENT0, this->Texture.ID, 0);
    if (glCheckNamedFramebufferStatus(this->FBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    }

    // Initialize render data and uniforms
    initRenderData();
    this->PostProcessingShader.SetInteger("scene", 0, true);
    float offset{ 1.0f / 300.0f };

    float offsets[9][2]{
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };
    glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);

    int edge_kernel[9]{
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
    
    float blur_kernel[9]{
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
    2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
    glBlitNamedFramebuffer(this->MSFBO, this->FBO, 0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(float time)
{
    // set uniforms/options
    this->PostProcessingShader.Use();
    this->PostProcessingShader.SetFloat("time", time);
    this->PostProcessingShader.SetInteger("confuse", this->Confuse);
    this->PostProcessingShader.SetInteger("chaos", this->Chaos);
    this->PostProcessingShader.SetInteger("shake", this->Shake);
    // render textured quad
    this->Texture.Bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[]{
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glCreateVertexArrays(1, &this->VAO);
    glCreateBuffers(1, &VBO);

    glNamedBufferStorage(VBO, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(this->VAO, 0, VBO, 0, sizeof(float) * 4);

    glEnableVertexArrayAttrib(this->VAO, 0);
    glVertexArrayAttribFormat(this->VAO, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(this->VAO, 0, 0);
}
