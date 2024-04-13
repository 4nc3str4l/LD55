#include "world.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "raymath.h"

inline const auto player_texture_path = "resources/player.png";
inline const auto ground_texture_path = "resources/ground.png";

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

Texture2D GetTextureFromPath(const std::string& path)
{
    Image image = LoadImage(path.c_str());
    auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

World LoadWorld(const std::string &worldPath, const std::string &entitiesPath)
{
    World world;
    auto data = LoadDataMatrix(worldPath, WORLD_WIDTH, WORLD_HEIGHT);
    auto entities = LoadDataMatrix(entitiesPath, WORLD_WIDTH, WORLD_HEIGHT);

    world.playerTexture = GetTextureFromPath(player_texture_path);
    world.groundTexture = GetTextureFromPath(ground_texture_path);


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
                world.tileTypes[x][y] = TileType::Dry;
                world.tileStates[x][y] = 0.0f;
                break;
            case 1:
                world.tiles[x][y] = grass_color;
                world.tileTypes[x][y] = TileType::Grass;
                world.tileStates[x][y] = 0.5f;
                break;
            case 2:
                world.tiles[x][y] = snow_color;
                world.tileTypes[x][y] = TileType::Grass;
                world.tileStates[x][y] = 1.0f;
                break;
            default:
                world.tiles[x][y] = RED;
                world.tileTypes[x][y] = TileType::None;
                world.tileStates[x][y] = 0.0f;
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
    BeginMode2D(world.camera);
    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            DrawTexture(world.groundTexture, x * TILE_SIZE, y * TILE_SIZE, world.tiles[x][y]);
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

    DrawTexture(world.playerTexture,
            world.player.position.x,
            world.player.position.y - TILE_SIZE,
            GREEN);


    Vector2 playerTilePos = GetTilePosition(world.player.position);

#ifdef _DEBUG
    DrawRectangle(playerTilePos.x * TILE_SIZE, playerTilePos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
#endif
    EndMode2D();
}

void UpdateWorldState(World &world, float deltaTime)
{
    for (const auto &elemental : world.elementals) {
        if (elemental.type == ElemetalType::None) continue;

        Vector2 elementalTilePos = GetTilePosition(elemental.position);
        int minX = std::max(0, static_cast<int>(elementalTilePos.x) - world.elementalRange);
        int maxX = std::min(WORLD_WIDTH, static_cast<int>(elementalTilePos.x) + world.elementalRange + 1);
        int minY = std::max(0, static_cast<int>(elementalTilePos.y) - world.elementalRange);
        int maxY = std::min(WORLD_HEIGHT, static_cast<int>(elementalTilePos.y) + world.elementalRange + 1);

        for (int y = minY; y < maxY; ++y) {
            for (int x = minX; x < maxX; ++x) {
                float distance = Vector2Distance(elementalTilePos, Vector2{static_cast<float>(x), static_cast<float>(y)});
                float influence = world.elementalRange - distance;
                if (influence < 0) continue;
                influence = influence * influence / (world.elementalRange * world.elementalRange) * world.elementalPower;
                if (elemental.type == ElemetalType::Fire) {
                    world.tileStates[x][y] = std::max(dry_range.x, world.tileStates[x][y] - influence * deltaTime);
                } else if (elemental.type == ElemetalType::Ice) {
                    world.tileStates[x][y] = std::min(snow_range.y, world.tileStates[x][y] + influence * deltaTime);
                }
            }
        }
    }
}

void UpdateTileStates(World &world, float deltaTime)
{
    int numGrassTiles = 0;
    for (int y = 0; y < WORLD_HEIGHT; y++)
    {
        for (int x = 0; x < WORLD_WIDTH; x++)
        {
            float tileState = world.tileStates[x][y];
            if (tileState <= dry_range.y) {
                world.tiles[x][y] = dry_color;
                world.tileTypes[x][y] = TileType::Dry;
            } else if (tileState <= grass_range.y) {
                world.tiles[x][y] = grass_color;
                world.tileTypes[x][y] = TileType::Grass;
            } else if (tileState <= snow_range.y) {
                world.tiles[x][y] = snow_color;
                world.tileTypes[x][y] = TileType::Snow;
            }

            if (world.tileTypes[x][y] == TileType::Grass) {
                numGrassTiles++;
            }
        }
    }
    world.springDominance = static_cast<float>(numGrassTiles) / (WORLD_WIDTH * WORLD_HEIGHT);
}

void UpdatePlayer(World &world, float deltaTime)
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

    // Calcula la nueva posición propuesta para el jugador
    Vector2 proposedPosition = {
        world.player.position.x + directionX * movementSpeed,
        world.player.position.y + directionY * movementSpeed
    };

    // Ajusta la posición propuesta para que no se salga de los límites del mundo
    proposedPosition.x = Clamp(proposedPosition.x, 0.0f, (WORLD_WIDTH - 1) * TILE_SIZE);
    proposedPosition.y = Clamp(proposedPosition.y, 0.0f, (WORLD_HEIGHT - 1) * TILE_SIZE);

    // Establece la posición ajustada del jugador
    world.player.position = proposedPosition;
}


void UpdateCamera(World *world, float deltaTime)
{
    world->camera.target = world->player.position;
    world->camera.offset = Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    world->camera.rotation = 0.0f;
    world->camera.zoom = 1.0f;
}

void UpdateWorld(World &world, float deltaTime)
{
    UpdatePlayer(world, deltaTime);
    UpdateWorldState(world, deltaTime);
    UpdateTileStates(world, deltaTime);
    UpdateCamera(&world, deltaTime);
}

Vector2 GetTilePosition(const Vector2& position)
{
    return Vector2{std::floor(position.x / TILE_SIZE), std::floor(position.y / TILE_SIZE)};
}
