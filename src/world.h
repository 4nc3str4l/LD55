#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include "ParticleSystem.h"

#define TILE_SIZE 32.0f
#define HALF_TILE_SIZE 16.0f

inline auto dry_color = Color{233, 178, 86, 255};
inline auto grass_color = Color{0, 255, 115, 255};
inline auto snow_color = Color{240, 240, 240, 255};

inline auto dry_range = Vector2{0.0f, 0.3f};
inline auto grass_range = Vector2{0.3f, 0.7f};
inline auto snow_range = Vector2{0.7f, 1.0f};

std::vector<std::vector<int>> LoadDataMatrix(const std::string &path, int &width, int &height);

inline constexpr int TIMES_INTIL_MOVEMENT_RADIUS_INCRESES = 20;

struct RegisteredWorld
{
    int level;
};

enum class ElementalType
{
    None = 0,
    Fire = 1,
    Ice = 2,
    Spring = 3,
    Count
};

enum class TileType
{
    None = -1,
    Dry = 0,
    Grass = 1,
    Snow = 2,
    Block = 3,
    Count
};

enum class EntityType
{
    Player = 1,
    FireElemental = 2,
    IceElemental = 3,
    SpringElemental = 4,
    SpringTotem = 5,
    SummerTotem = 6,
    WinterTotem = 7,
    Count
};

enum class ElementalStatus
{
    Idle = 0,
    Moving = 1,
    Grabbed = 2,
    Count
};

enum class PlayerStatus
{
    Idle = 0,
    Moving = 1,
    Grabbing = 2,
    Count
};

struct MortalEntity 
{    
    float initialHealth = 100.0f;
    float health = 100.0f;
    float damageRate = 5.0f;
    float nextHealthCheck = 0.0f;
    float checkRate = 2.0f;
    bool isDead = false;
};

struct Player
{
    Vector2 position = {100, 100};
    float speed = 300.0f;
    PlayerStatus status = PlayerStatus::Moving;
    MortalEntity mortalEntity = {100.0f, 100.0f, 5.0f, 0.0f, 2.0f, false};
};


struct Elemental
{
    Vector2 position{};
    ElementalType type = ElementalType::None;
    int movementRadius = 1;
    float speed = 20.0f;
    int timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
    Vector2 ChoosenPosition = {0, 0};
    ElementalStatus status = ElementalStatus::Moving;
};

enum class GameStatus
{
    Starting = 0,
    Playing = 1,
    GameOver = 2,
    LevelComplete = 2,
};

struct TutorialText
{
    Vector2 position;
    bool isUi = true;
    std::string text;
};

struct Block
{
    Vector2 position;
};

struct World
{
    int currentLevel = 1;
    int width = 0;
    int height = 0;

    std::vector<std::vector<Color>> tiles;
    std::vector<std::vector<TileType>> tileTypes;
    std::vector<std::vector<float>> tileStates;
    std::vector<Elemental> elementals;
    std::vector<TutorialText> tutorialTexts;
    std::vector<Block> blocks;

    Player player = Player();

    Texture2D playerTexture{};
    Texture2D groundTexture{};
    Texture2D springStaffTexture{};

    Texture2D fireElementalTexture{};
    Texture2D iceElementalTexture{};

    Texture2D fireElementalCaptiveTexture{};
    Texture2D iceElementalCaptiveTexture{};

    Texture2D blockTexture{};

    float elementalPower = 0.1f;
    int elementalRange = 4;
    Camera2D camera = {0};
    int springTiles = 0;
    float springDominance = 0.0f;

    ParticleSystem particleSystem;
    bool firstTileComputed = false;
};

World *LoadWorld(int level,
                 const std::string &worldPath,
                 const std::string &entitiesPath,
                 const std::string &tutorialPath);
void DeleteWorld(World *world);
std::vector<TutorialText> LoadTutorialText(const std::string &path);
void RenderWorld(World *world, Shader *distortionShader, Shader *entitiesShader);
void UpdateWorld(World *world, float deltaTime);
Vector2 GetTilePosition(const Vector2 &position);
void NotifyStateChange(World *world, Rectangle where, TileType from, TileType to);
void NotifyPlayerHealthChange(World *world, float lastHealth, float newHealth);