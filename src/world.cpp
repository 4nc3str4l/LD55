#include "world.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

std::vector<std::vector<int>> LoadDataMatrix(const std::string& path, int width, int height)
{
    std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));
    std::ifstream file(path);
    std::string line;
    int currentLine = 0;

    if (file.is_open()) {
        while (getline(file, line) && currentLine < height) {
            std::istringstream iss(line);
            std::string token;
            int numIndex = 0;
            while (getline(iss, token, ',') && numIndex < width) {
                matrix[currentLine][numIndex] = std::stoi(token);
                std::cout << matrix[currentLine][numIndex] << " ";
                numIndex++;
            }
            std::cout << std::endl;

            currentLine++;
        }
        file.close();
    } else {
        std::cerr << "Failed to open the file\n";
    }

#ifdef _DEBUG
    std::cout << "Matrix loaded from " << path << std::endl;
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
#endif

    return matrix;
}

World LoadWorld(const std::string &worldPath, const std::string &entitiesPath)
{
    World world;
    auto data = LoadDataMatrix(worldPath, WORLD_WIDTH, WORLD_HEIGHT);
    auto entities = LoadDataMatrix(entitiesPath, WORLD_WIDTH, WORLD_HEIGHT);

    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            auto tile = data[y][x];
            world.tileTypes[x][y] = static_cast<TileType>(tile);
            switch (tile)
            {
            case 0:
                world.tiles[x][y] = dry_color;
                break;
            case 1:
                world.tiles[x][y] = grass_color;
                break;
            case 2:
                world.tiles[x][y] = snow_color;
                break;
            default:
                world.tiles[x][y] = RED;
                break;
            }

            auto entity = entities[y][x];
            auto position = Vector2{x * TILE_SIZE + HALF_TILE_SIZE, y * TILE_SIZE + HALF_TILE_SIZE};
            if (entity == 1)
            {
                world.player.position = position;
            }
            else if (entity == 2)
            {
                world.elementals[y * WORLD_WIDTH + x] = Elemental{position, ElemetalType::Fire};
            }
            else if (entity == 3)
            {
                world.elementals[y * WORLD_WIDTH + x] = Elemental{position, ElemetalType::Ice};
            }
        }
    }

    return world;
}

void RenderWorld(const World &world)
{
    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, world.tiles[x][y]);
        }
    }

    for (int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++)
    {
        auto elemental = world.elementals[i];
        if (elemental.type == ElemetalType::Fire)
        {
            DrawRectangle(elemental.position.x - HALF_TILE_SIZE, elemental.position.y - HALF_TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
        }
        else if (elemental.type == ElemetalType::Ice)
        {
            DrawRectangle(elemental.position.x - HALF_TILE_SIZE, elemental.position.y - HALF_TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
        }
    }
}

void UpdateWorld(World &world, float deltaTime)
{
    float directionX = 0.0f;
    float directionY = 0.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        directionX += 1.0f;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        directionX -= 1.0f;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        directionY += 1.0f;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        directionY -= 1.0f;
    }

    float length = sqrtf(directionX * directionX + directionY * directionY);
    if (length != 0.0f) {
        directionX /= length;
        directionY /= length;
    }

    float movementSpeed = world.player.speed * deltaTime;
    world.player.position.x += directionX * movementSpeed;
    world.player.position.y += directionY * movementSpeed;
}
