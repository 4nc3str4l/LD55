#pragma once

#include "raylib.h"

#define WORLD_WIDTH  32
#define WORLD_HEIGHT 18


inline auto dry_color = Color{233, 178, 86, 255 };
inline auto grass_color = Color{ 0, 255, 115, 255 };
inline auto snow_color = Color{ 240, 240, 240, 255 };

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

struct Player {
    int x;
    int y;
};

struct Elemental {
    int x;
    int y;
    ElemetalType type;
};

struct World {
    Color tiles[WORLD_WIDTH][WORLD_HEIGHT];
    TileType tileTypes[WORLD_WIDTH][WORLD_HEIGHT];
    Player player;
    Elemental elementals[WORLD_WIDTH * WORLD_HEIGHT];
};