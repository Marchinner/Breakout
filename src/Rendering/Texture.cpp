#include "Rendering/Texture.h"
#include <glad/glad.h>
#include <iostream>

Texture2D::Texture2D()
    : ID{ 0 }
    , Width{ 0 }
    , Height{ 0 }
    , InternalFormat{ GL_RGB8 }
    , ImageFormat{ GL_RGB }
    , WrapS{ GL_REPEAT }
    , WrapT{ GL_REPEAT }
    , FilterMin{ GL_LINEAR }
    , FilterMax{ GL_LINEAR }
{
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;

    glCreateTextures(GL_TEXTURE_2D, 1, &this->ID);

    // Allocate texture storage
    glTextureStorage2D(this->ID, 1, this->InternalFormat, width, height);

    // Set texture wrap and filter modes
    glTextureParameteri(this->ID, GL_TEXTURE_WRAP_S, this->WrapS);
    glTextureParameteri(this->ID, GL_TEXTURE_WRAP_T, this->WrapT);
    glTextureParameteri(this->ID, GL_TEXTURE_MIN_FILTER, this->FilterMin);
    glTextureParameteri(this->ID, GL_TEXTURE_MAG_FILTER, this->FilterMax);

    glTextureSubImage2D(this->ID, 0, 0, 0, width, height, this->ImageFormat, GL_UNSIGNED_BYTE, data);
}

void Texture2D::Bind() const
{
    glBindTextureUnit(0, this->ID);

    // Check if the texture is bound
    int boundTextureID = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTextureID);
    if (boundTextureID != static_cast<int>(this->ID)) {
        std::cerr << "Failed to bind texture." << std::endl;
    }
}
