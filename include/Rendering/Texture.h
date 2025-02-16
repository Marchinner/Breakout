#pragma once
class Texture2D
{
public:
    // Constructor
    Texture2D();

    // Generate texture from file
    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    // Bind texture
    void Bind() const;

public:
    // Holds the ID of the texture
    unsigned int ID;

    // Texture image dimensions
    unsigned int Width, Height;

    // Texture Format
    unsigned int InternalFormat;
    unsigned int ImageFormat;

    // Texture configuration
    unsigned int WrapS;
    unsigned int WrapT;
    unsigned int FilterMin;
    unsigned int FilterMax;
};

