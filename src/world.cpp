#include "world.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "raymath.h"
#include <algorithm>
#include "utils.h"

inline const auto player_texture_path = "resources/player.png";
inline const auto ground_texture_path = "resources/ground.png";

#define _DEBUG

std::vector<std::vector<int>> LoadDataMatrix(const std::string& path, int& width, int& height) {
    std::ifstream file(path);
    std::vector<std::vector<int>> matrix;
    std::string line;
    height = 0;

    if (!file.is_open()) {
        std::cerr << "Failed to open the file\n";
        width = 0;
        height = 0;
        return matrix;
    }

    int maxColumns = 0;
    while (getline(file, line)) {
        std::vector<int> row;
        std::istringstream iss(line);
        std::string token;

        while (getline(iss, token, ',')) {
            row.push_back(std::stoi(token));
        }

        maxColumns = std::max(maxColumns, static_cast<int>(row.size()));
        matrix.push_back(row);
        height++;
    }

    file.close();
    width = maxColumns;

    for (auto& row : matrix) {
        row.resize(width, 0);
    }

#ifdef _DEBUG
    std::cout << "Matrix loaded from " << path << " with dimensions: " << width << "x" << height << std::endl;
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

    int width = 0;
    int height = 0;

    auto data = LoadDataMatrix(worldPath, width, height);
    auto entities = LoadDataMatrix(entitiesPath, width, height);

    world.width = width;
    world.height = height;

    world.elementals.resize(width * height);
    world.tiles.resize(height, std::vector<Color>(width));
    world.tileTypes.resize(height, std::vector<TileType>(width));
    world.tileStates.resize(height, std::vector<float>(width));

    world.playerTexture = GetTextureFromPath(player_texture_path);
    world.groundTexture = GetTextureFromPath(ground_texture_path);

    for (int y = 0; y < world.height; y++)
    {
        for (int x = 0; x < world.width; x++)
        {
            auto tile = data[y][x];
            world.tileTypes[y][x] = static_cast<TileType>(tile);
            switch (tile)
            {
            case 0:
                world.tiles[y][x] = dry_color;
                world.tileTypes[y][x] = TileType::Dry;
                world.tileStates[y][x] = 0.0f;
                break;
            case 1:
                world.tiles[y][x] = grass_color;
                world.tileTypes[y][x] = TileType::Grass;
                world.tileStates[y][x] = 0.5f;
                break;
            case 2:
                world.tiles[y][x] = snow_color;
                world.tileTypes[y][x] = TileType::Grass;
                world.tileStates[y][x] = 1.0f;
                break;
            default:
                world.tiles[y][x] = RED;
                world.tileTypes[y][x] = TileType::None;
                world.tileStates[y][x] = 0.0f;
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
                auto elemental = Elemental{position, ElemetalType::Fire};
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                world.elementals[y * world.width + x] = elemental;

            }
            else if (entity == 3)
            {
                auto elemental = Elemental{position, ElemetalType::Ice};
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                world.elementals[y * world.width + x] = elemental;
            }
        }
    }

    return world;
}

void RenderWorld(const World &world)
{
    BeginMode2D(world.camera);
    for (int y = 0; y < world.height; y++)
    {
        for (int x = 0; x < world.width; x++)
        {
            DrawTexture(world.groundTexture, x * TILE_SIZE, y * TILE_SIZE, world.tiles[y][x]);
        }
    }

    for (int i = 0; i < world.width * world.height; i++)
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
        if (elemental.status == ElementalStatus::Grabbed) continue;

        Vector2 elementalTilePos = GetTilePosition(elemental.position);
        int minX = std::max(0, static_cast<int>(elementalTilePos.x) - world.elementalRange);
        int maxX = std::min(world.width, static_cast<int>(elementalTilePos.x) + world.elementalRange + 1);
        int minY = std::max(0, static_cast<int>(elementalTilePos.y) - world.elementalRange);
        int maxY = std::min(world.height, static_cast<int>(elementalTilePos.y) + world.elementalRange + 1);

        for (int y = minY; y < maxY; ++y) {
            for (int x = minX; x < maxX; ++x) {
                float distance = Vector2Distance(elementalTilePos, Vector2{static_cast<float>(x), static_cast<float>(y)});
                float influence = world.elementalRange - distance;
                if (influence < 0) continue;
                influence = influence * influence / (world.elementalRange * world.elementalRange) * world.elementalPower;
                if (elemental.type == ElemetalType::Fire) {
                    world.tileStates[y][x] = std::max(dry_range.x, world.tileStates[y][x] - influence * deltaTime);
                } else if (elemental.type == ElemetalType::Ice) {
                    world.tileStates[y][x] = std::min(snow_range.y, world.tileStates[y][x] + influence * deltaTime);
                }
            }
        }
    }
}

void UpdateTileStates(World &world, float deltaTime)
{
    int numGrassTiles = 0;
    for (int y = 0; y < world.height; y++)
    {
        for (int x = 0; x < world.width; x++)
        {
            float tileState = world.tileStates[y][x];
            if (tileState <= dry_range.y) {
                world.tiles[y][x] = dry_color;
                world.tileTypes[y][x] = TileType::Dry;
            } else if (tileState <= grass_range.y) {
                world.tiles[y][x] = grass_color;
                world.tileTypes[y][x] = TileType::Grass;
            } else if (tileState <= snow_range.y) {
                world.tiles[y][x] = snow_color;
                world.tileTypes[y][x] = TileType::Snow;
            }

            if (world.tileTypes[y][x] == TileType::Grass) {
                numGrassTiles++;
            }
        }
    }
    world.springDominance = static_cast<float>(numGrassTiles) / (world.width * world.height);
    world.springTiles = numGrassTiles;
}

void UpdatePlayer(World &world, float deltaTime)
{
    // Dont allow player to move if it is dead or idle
    if(world.player.status == PlayerStatus::Dead ||
         world.player.status == PlayerStatus::Idle) return;

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

    Vector2 proposedPosition = {
        world.player.position.x + directionX * movementSpeed,
        world.player.position.y + directionY * movementSpeed
    };

    proposedPosition.x = Clamp(proposedPosition.x, 0.0f, (world.width - 1) * TILE_SIZE);
    proposedPosition.y = Clamp(proposedPosition.y, 0.0f, (world.height - 1) * TILE_SIZE);

    world.player.position = proposedPosition;

    if (IsKeyPressed(KEY_SPACE)) {
        if (world.player.status == PlayerStatus::Moving) {
            for (auto &elemental : world.elementals) {
                float distance = Vector2Distance(world.player.position, elemental.position);
                if (distance < TILE_SIZE * 2 && elemental.status == ElementalStatus::Moving) {
                    elemental.status = ElementalStatus::Grabbed;
                    world.player.status = PlayerStatus::Grabbing;
                    break;
                    }
            }
        } else if (world.player.status == PlayerStatus::Grabbing) {
            for (auto &elemental : world.elementals) {
                if (elemental.status == ElementalStatus::Grabbed) {
                    elemental.status = ElementalStatus::Moving;
                    elemental.movementRadius = 1;
                    elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                    world.player.status = PlayerStatus::Moving;
                    elemental.ChoosenPosition = elemental.position;
                    break;
                }
            }
        }
    }

}


void UpdateCamera(World *world, float deltaTime)
{
    world->camera.target = world->player.position;
    world->camera.offset = Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    world->camera.rotation = 0.0f;
    world->camera.zoom = 1.0f;
}

void UpdateElementals(World& world, float deltaTime) {
    for (auto& elemental : world.elementals) {
        if (elemental.type == ElemetalType::None) continue;

        if(elemental.status == ElementalStatus::Grabbed){
            elemental.position = world.player.position;
            continue;
        }

        Vector2 direction = Vector2Subtract(elemental.ChoosenPosition, elemental.position);
        float distance = Vector2Length(direction);

        if (distance > 0) {
            Vector2 movement = Vector2Scale(Vector2Normalize(direction), std::min(elemental.speed * deltaTime, distance));
            elemental.position = Vector2Add(elemental.position, movement);
        }

        if (distance < elemental.speed * deltaTime) {

            float minX = std::max(0.0f, elemental.position.x - elemental.movementRadius * TILE_SIZE);
            float maxX = std::min((world.width - 1) * TILE_SIZE, elemental.position.x + elemental.movementRadius * TILE_SIZE);
            float minY = std::max(0.0f, elemental.position.y - elemental.movementRadius * TILE_SIZE);
            float maxY = std::min((world.height - 1) * TILE_SIZE, elemental.position.y + elemental.movementRadius * TILE_SIZE);

            elemental.ChoosenPosition = GetRandomVector(minX, minY, maxX, maxY);

            if (--elemental.timesUntilMovementIncrease <= 0) {
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.movementRadius++;
            }
        }
    }
}

void UpdateWorld(World &world, float deltaTime)
{
    UpdatePlayer(world, deltaTime);
    UpdateWorldState(world, deltaTime);
    UpdateElementals(world, deltaTime);
    UpdateTileStates(world, deltaTime);
    UpdateCamera(&world, deltaTime);
}

Vector2 GetTilePosition(const Vector2& position)
{
    return Vector2{std::floor(position.x / TILE_SIZE), std::floor(position.y / TILE_SIZE)};
}
