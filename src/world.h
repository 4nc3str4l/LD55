#pragma once

#include "raylib.h"
#include <vector>
#include <string>

#define TILE_SIZE 32.0f
#define HALF_TILE_SIZE 16.0f

inline auto dry_color = Color{233, 178, 86, 255 };
inline auto grass_color = Color{ 0, 255, 115, 255 };
inline auto snow_color = Color{ 240, 240, 240, 255 };

inline auto dry_range = Vector2{0.0f, 0.3f};
inline auto grass_range = Vector2{0.3f, 0.7f};
inline auto snow_range = Vector2{0.7f, 1.0f};

std::vector<std::vector<int>> LoadDataMatrix(const std::string& path, int& width, int& height);

inline constexpr int TIMES_INTIL_MOVEMENT_RADIUS_INCRESES = 4;

enum class ElemetalType {
    None = 0,
    Fire = 1,
    Ice = 2,
    Spring = 3,
};

enum class TileType {
    None = -1,
    Dry = 0,
    Grass = 1,
    Snow = 2,
    Count = 3
};

enum class EntityType {
    Player = 1,
    FireElemental = 2,
    IceElemental = 3,
    SpringElemental = 4,
    SpringTotem = 5,
    SummerTotem = 6,
    WinterTotem = 7,
    Count
};

enum class ElementalStatus {
    Idle = 0,
    Moving = 1,
    Grabbed = 2,
};

enum class PlayerStatus {
    Idle = 0,
    Moving = 1,
    Grabbing = 2,
    Dead = 3,
};

struct Player {
    Vector2 position = { 100, 100 };
    float speed = 100.0f;
    PlayerStatus status = PlayerStatus::Moving;
};

struct Elemental {
    Vector2 position;
    ElemetalType type;
    int movementRadius = 1;
    float speed = 50.0f;
    int timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
    Vector2 ChoosenPosition = {0, 0};
    ElementalStatus status = ElementalStatus::Moving;
};

struct TutorialText {
    Vector2 position;
    bool isUi = true;
    std::string text;
};

struct World {
    int currentLevel = 1;
    int width = 0;
    int height = 0;

    std::vector<std::vector<Color>> tiles;
    std::vector<std::vector<TileType>> tileTypes;
    std::vector<std::vector<float>> tileStates;
    std::vector<Elemental> elementals;
    std::vector<TutorialText> tutorialTexts;

    Player player = Player();

    Texture2D playerTexture{};
    Texture2D groundTexture{};

    float elementalPower = 0.1f;
    int elementalRange = 4;
    Camera2D camera = {0};
    int springTiles = 0;
    float springDominance = 0.0f;
};

World LoadWorld(const std::string &worldPath,
                const std::string &entitiesPath,
                const std::string &tutorialPath);
std::vector<TutorialText> LoadTutorialText(const std::string &path);
void RenderWorld(const World &world);
void UpdateWorld(World &world, float deltaTime);
Vector2 GetTilePosition(const Vector2 &position);