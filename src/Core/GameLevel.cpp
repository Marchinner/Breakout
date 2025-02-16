#include "Core/GameLevel.h"

#include <string>
#include <fstream>
#include <sstream>
#include <Core/ResourceManager.h>

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
    // Clear old data
    this->Bricks.clear();

    // Load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        // Read each line from level file
        while (std::getline(fstream, line))
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;

            // Read each word separated by spaces
            while (sstream >> tileCode)
            {
                row.push_back(tileCode);
            }

            tileData.push_back(row);
        }

        if (tileData.size() > 0)
        {
            this->init(tileData, levelWidth, levelHeight);
        }
    }
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& tile : this->Bricks)
    {
        if (!tile.Destroyed)
        {
            tile.Draw(renderer);
        }
    }
}

bool GameLevel::IsCompleted()
{
    for (GameObject& tile : this->Bricks)
    {
        if (!tile.IsSolid && !tile.Destroyed)
        {
            return false;
        }
    }

    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // Calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();
    float unitWidth = levelWidth / static_cast<float>(width);
    float unitHeight = levelHeight / height;

    // Initialize level tiles based on tileData
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            // Check block type from level data (2D level array)
            if (tileData[y][x] == 1) // Solid
            {
                glm::vec2 pos{ unitWidth * x, unitHeight * y };
                glm::vec2 size{ unitWidth, unitHeight };

                GameObject obj{ pos, size, ResourceManager::GetTexture("block_solid"),
                glm::vec3{0.8f, 0.8f, 0.7f} };

                obj.IsSolid = true;
                
                this->Bricks.push_back(obj);
            }
            else if (tileData[y][x] > 1)
            {
                glm::vec3 color{ glm::vec3{1.0f} }; // Original: white

                if (tileData[y][x] == 2)
                {
                    color = glm::vec3{ 0.2f, 0.6f, 1.0f };
                }
                else if (tileData[y][x] == 3)
                {
                    color = glm::vec3{ 0.0f, 0.7f, 0.0f };
                }
                else if (tileData[y][x] == 4)
                {
                    color = glm::vec3{ 0.8f, 0.8f, 0.4f };
                }
                else if (tileData[y][x] == 5)
                {
                    color = glm::vec3{ 1.0f, 0.5f, 0.0f };
                }

                glm::vec2 pos{ unitWidth * x, unitHeight * y };
                glm::vec2 size{ unitWidth, unitHeight };

                this->Bricks.push_back(GameObject{ pos, size, ResourceManager::GetTexture("block"), color });
            }
        }
    }
}
