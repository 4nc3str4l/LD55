#pragma once

#include "raylib.h"
#include <vector>
#include <string>

#define WORLD_WIDTH  32
#define WORLD_HEIGHT 18

#define TILE_SIZE 32.0f
#define HALF_TILE_SIZE 16.0f


inline auto dry_color = Color{233, 178, 86, 255 };
inline auto grass_color = Color{ 0, 255, 115, 255 };
inline auto snow_color = Color{ 240, 240, 240, 255 };

std::vector<std::vector<int>> LoadDataMatrix(const std::string& path, int width, int height);

enum class ElemetalType {
    None = 0,
    Fire = 1,
    Ice = 2,
};

enum class TileType {
    Dry = 0,
    Grass = 1,
    Snow = 2,
    Count = 3
};

enum class EntityType {
    Player = 1,
    FireElemental = 2,
    IceElemental = 3,
    SpringTotem = 4,
    SummerTotem = 5,
    WinterTotem = 6,
};

struct Player {
    Vector2 position = { 0, 0 };
    float speed = 100.0f;
};

struct Elemental {
    Vector2 position;
    ElemetalType type;
};

struct World {
    Color tiles[WORLD_WIDTH][WORLD_HEIGHT]{};
    TileType tileTypes[WORLD_WIDTH][WORLD_HEIGHT]{};
    Player player = Player();
    Elemental elementals[WORLD_WIDTH * WORLD_HEIGHT]{};
};

World LoadWorld(const std::string &worldPath, const std::string &entitiesPath);
void RenderWorld(const World &world);
void UpdateWorld(World &world, float deltaTime);