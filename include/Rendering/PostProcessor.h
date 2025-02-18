#pragma once
#include "Shader.h"
#include "Texture.h"


class PostProcessor
{
public:
    PostProcessor(Shader shader, unsigned int width, unsigned height);
    void BeginRender();
    void EndRender();
    void Render(float time);

public:
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width;
    unsigned int Height;
    // Options
    bool Confuse;
    bool Chaos;
    bool Shake;

private:
    void initRenderData();

private:
    unsigned int MSFBO; // Multisampled FBO. 
    unsigned int FBO;   // FBO is regular, used for blitting MS color-buffer to texture
    unsigned int RBO;   // RBO is used for multisampled color buffer
    unsigned int VAO;
};

