#pragma once
#include <map>
#include <string>


#include <Rendering/Texture.h>
#include <Rendering/Shader.h>
class ResourceManager
{
public:
    // Resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;

    // Loads and generates a shader program from file loading vertex, fragment (and geometry) shader's source code. If geometry shader is not nullptr, it is also loaded
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);

    // Retrieves a stored shader
    static Shader& GetShader(std::string name);

    // Loads and generates a texture from file
    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

    // Retrieves a stored texture
    static Texture2D& GetTexture(std::string name);

    // Properly de-allocates all loaded resources
    static void Clear();

private:
    // Private constructor, that is we do not want any actual resource manager objects. Its memory is static and globally available.
    ResourceManager() {}

    // Loads and generates a shader from file
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

    // Loads a single texture from file
    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

