#include "world.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "raymath.h"
#include <algorithm>
#include "utils.h"
#include <limits>
#include "FxManager.h"
#include "SoundManager.h"

inline const auto player_texture_path = "resources/player.png";
inline const auto ground_texture_path = "resources/ground.png";

std::vector<std::vector<int>> LoadDataMatrix(const std::string &path, int &width, int &height)
{
    std::ifstream file(path);
    std::vector<std::vector<int>> matrix;
    std::string line;
    height = 0;

    if (!file.is_open())
    {
        std::cerr << "Failed to open the file\n";
        width = 0;
        height = 0;
        return matrix;
    }

    int maxColumns = 0;
    while (getline(file, line))
    {
        std::vector<int> row;
        std::istringstream iss(line);
        std::string token;

        while (getline(iss, token, ','))
        {
            row.push_back(std::stoi(token));
        }

        maxColumns = std::max(maxColumns, static_cast<int>(row.size()));
        matrix.push_back(row);
        height++;
    }

    file.close();
    width = maxColumns;

    for (auto &row : matrix)
    {
        row.resize(width, 0);
    }

#ifdef _DEBUG
    std::cout << "Matrix loaded from " << path << " with dimensions: " << width << "x" << height << std::endl;
    for (const auto &row : matrix)
    {
        for (const auto &elem : row)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
#endif

    return matrix;
}

Texture2D GetTextureFromPath(const std::string &path)
{
    Image image = LoadImage(path.c_str());
    auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

std::vector<TutorialText> LoadTutorialText(const std::string &path)
{
    std::vector<TutorialText> tutorials;
    std::ifstream file(path);
    std::string line;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            std::istringstream iss(line);
            std::string type;
            int posX, posY;
            std::string text;

            if (std::getline(iss, type, ',') && (iss >> posX) && iss.ignore(256, ',') && (iss >> posY))
            {
                std::getline(std::getline(iss, text, ']'), text);

                text.erase(0, text.find_first_not_of(" \t"));

                TutorialText tutorialText;
                tutorialText.position = Vector2{static_cast<float>(posX), static_cast<float>(posY)};
                tutorialText.text = text;

                if (type.find("[u") != std::string::npos)
                {
                    tutorialText.isUi = true;
                }
                else if (type.find("[w") != std::string::npos)
                {
                    tutorialText.isUi = false;
                }
                else
                {
                    std::cout << "Unknown type prefix: " << type << ". Line: " << line << std::endl;
                    continue;
                }

                tutorials.push_back(tutorialText);
            }
            else
            {
                std::cout << "Failed to parse line: " << line << std::endl;
            }
        }
        file.close();
    }
    else
    {
        std::cout << "Unable to open file: " << path << std::endl;
    }

    std::cout << "Num tutorials loaded: " << tutorials.size() << std::endl;
    return tutorials;
}

World *LoadWorld(int level,
                 const std::string &worldPath,
                 const std::string &entitiesPath,
                 const std::string &tutorialPath)
{
    FXManager::Init();
    auto world = new World();
    world->currentLevel = level;
    int width = 0;
    int height = 0;

    auto data = LoadDataMatrix(worldPath, width, height);
    auto entities = LoadDataMatrix(entitiesPath, width, height);
    // Load tutorials if the file exists
    world->tutorialTexts = LoadTutorialText(tutorialPath);
    world->width = width;
    world->height = height;

    world->tiles.resize(height, std::vector<Color>(width));
    world->tileTypes.resize(height, std::vector<TileType>(width));
    world->tileStates.resize(height, std::vector<float>(width));

    world->playerTexture = GetTextureFromPath(player_texture_path);
    world->groundTexture = GetTextureFromPath(ground_texture_path);
    world->springStaffTexture = GetTextureFromPath("resources/spring_staff.png");
    world->fireElementalTexture = GetTextureFromPath("resources/fire_elemental_free.png");
    world->iceElementalTexture = GetTextureFromPath("resources/ice_elemental_free.png");
    world->fireElementalCaptiveTexture = GetTextureFromPath("resources/fire_elemental_captive.png");
    world->iceElementalCaptiveTexture = GetTextureFromPath("resources/ice_elemental_captive.png");
    world->blockTexture = GetTextureFromPath("resources/block.png");

    world->fireStaffTexture = GetTextureFromPath("resources/fire_staff.png");
    world->iceStaffTexture = GetTextureFromPath("resources/ice_staff.png");

    world->fireGemTexture = GetTextureFromPath("resources/fire_gem.png");
    world->iceGemTexture = GetTextureFromPath("resources/ice_gem.png");

    int numBlocks = 0;
    for (int y = 0; y < world->height; y++)
    {
        for (int x = 0; x < world->width; x++)
        {
            auto tile = data[y][x];
            world->tileTypes[y][x] = static_cast<TileType>(tile);
            switch (tile)
            {
            case 0:
                world->tiles[y][x] = dry_color;
                world->tileTypes[y][x] = TileType::Dry;
                world->tileStates[y][x] = 0.0f;
                break;
            case 1:
                world->tiles[y][x] = grass_color;
                world->tileTypes[y][x] = TileType::Grass;
                world->tileStates[y][x] = 0.5f;
                break;
            case 2:
                world->tiles[y][x] = snow_color;
                world->tileTypes[y][x] = TileType::Snow;
                world->tileStates[y][x] = 1.0f;
                break;
            case 3:
                world->blocks.push_back({Vector2{x * TILE_SIZE, y * TILE_SIZE}});
                world->tileTypes[y][x] = TileType::Block;
                world->tileStates[y][x] = 0.5f;
                numBlocks++;
                break;

            default:
                world->tiles[y][x] = RED;
                world->tileTypes[y][x] = TileType::None;
                world->tileStates[y][x] = 0.0f;
                break;
            }

            auto entity = entities[y][x];

            auto position = Vector2{x * TILE_SIZE + HALF_TILE_SIZE, y * TILE_SIZE + HALF_TILE_SIZE};
            if (entity == 1)
            {
                world->player.position = position;
            }
            else if (entity == 2)
            {
                auto elemental = Elemental{position, ElementalType::Fire};
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                world->elementals.emplace_back(elemental);
            }
            else if (entity == 3)
            {
                auto elemental = Elemental{position, ElementalType::Ice};
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                world->elementals.emplace_back(elemental);
            }
            else if (entity == 4)
            {
                auto elemental = Elemental{position, ElementalType::Spring};
                elemental.movementRadius = 0;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                elemental.speed = 0.0f;
                world->elementals.emplace_back(elemental);
            }
            else if (entity == 5)
            {
                auto elemental = Elemental{position, ElementalType::FireStaff};
                elemental.movementRadius = 0;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                elemental.speed = 0.0f;
                world->elementals.emplace_back(elemental);
            }
            else if (entity == 6)
            {
                auto elemental = Elemental{position, ElementalType::IceStaff};
                elemental.movementRadius = 0;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                elemental.speed = 0.0f;
                world->elementals.emplace_back(elemental);
            }
        }
    }

    std::cout << "Num blocks: " << numBlocks << std::endl;

    return world;
}

void RenderVictoryWorld(World *world, Shader *distortionShader, Shader *entitiesShader)
{
    // Render the player
    BeginMode2D(world->camera);

    for (int y = 0; y < world->height; y++)
    {
        for (int x = 0; x < world->width; x++)
        {
            float sineWave = sinf((x + world->timeInVictory * 10.0f) * 0.5f);
            float yOffset = (world->height - y + sineWave * 5.0f) * TILE_SIZE * (1.0f - world->timeInVictory);

            yOffset = fmaxf(0.0f, yOffset);
            DrawTexture(world->groundTexture, x * TILE_SIZE, y * TILE_SIZE - yOffset, world->tiles[y][x]);
        }
    }

    // Same for blocks
    for (const auto &block : world->blocks)
    {
        // Añadir una oscilación basada en seno que dependa de la posición x, y y el tiempo
        float sineWave = sinf((block.position.x / TILE_SIZE + world->timeInVictory * 10.0f) * 0.5f);
        float yOffset = ((world->height - block.position.y / TILE_SIZE) + sineWave * 5.0f) * TILE_SIZE * (1.0f - world->timeInVictory);

        // Asegúrate de que yOffset es positivo para que los bloques "vuelen" hacia arriba
        yOffset = fmaxf(0.0f, yOffset);

        DrawTexture(world->blockTexture, block.position.x, block.position.y - yOffset, WHITE);
    }
    BeginShaderMode(*entitiesShader);

    Vector4 tintVector = {
        GREEN.r / 255.0f,
        GREEN.g / 255.0f,
        GREEN.b / 255.0f,
        fmax(world->player.mortalEntity.health / world->player.mortalEntity.initialHealth, 0.05f),
    };

    SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
    DrawTexture(world->playerTexture,
                world->player.position.x,
                world->player.position.y - TILE_SIZE,
                GREEN);
    EndShaderMode();

    for (const auto &elemental : world->elementals)
    {

        if (elemental.type == ElementalType::Fire)
        {
            DrawTexture(world->fireElementalCaptiveTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
        }
        else if (elemental.type == ElementalType::Ice)
        {
            DrawTexture(world->iceElementalCaptiveTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
        }
        else if (elemental.type == ElementalType::Spring)
        {
            BeginShaderMode(*entitiesShader);
            Vector4 tintVector = {
                1, 1, 1, 1};
            SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
            DrawTexture(world->springStaffTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            EndShaderMode();
        }
        else if (elemental.type == ElementalType::FireStaff)
        {
            BeginShaderMode(*entitiesShader);
            Vector4 tintVector = {
                1, 1, 1, 1};
            SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
            DrawTexture(world->fireStaffTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            EndShaderMode();
        }
        else if (elemental.type == ElementalType::IceStaff)
        {
            BeginShaderMode(*entitiesShader);
            Vector4 tintVector = {
                1, 1, 1, 1};
            SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
            DrawTexture(world->iceStaffTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            EndShaderMode();
        }
    }
    EndMode2D();
}

void RenderWorld(World *world, Shader *distortionShader, Shader *entitiesShader)
{

    if (VictoryCondition(world))
    {
        RenderVictoryWorld(world, distortionShader, entitiesShader);
        return;
    }

    BeginMode2D(world->camera);
    for (int y = 0; y < world->height; y++)
    {
        for (int x = 0; x < world->width; x++)
        {
            DrawTexture(world->groundTexture, x * TILE_SIZE, y * TILE_SIZE, world->tiles[y][x]);
        }
    }

    for (const auto &block : world->blocks)
    {
        DrawTexture(world->blockTexture, block.position.x, block.position.y, WHITE);
    }

    BeginShaderMode(*entitiesShader);

    Vector4 tintVector = {
        GREEN.r / 255.0f,
        GREEN.g / 255.0f,
        GREEN.b / 255.0f,
        fmax(world->player.mortalEntity.health / world->player.mortalEntity.initialHealth, 0.05f),
    };

    SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
    DrawTexture(world->playerTexture,
                world->player.position.x,
                world->player.position.y - TILE_SIZE,
                GREEN);
    EndShaderMode();

    for (const auto &elemental : world->elementals)
    {

        if (elemental.type == ElementalType::Fire)
        {
            if (elemental.status == ElementalStatus::Grabbed)
            {
                DrawTexture(world->fireElementalCaptiveTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            }
            else
            {
                BeginShaderMode(*entitiesShader);
                Vector4 tintVector = {1, 1, 1, 1};
                SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);

                DrawTexture(world->fireElementalTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
                EndShaderMode();
            }
        }
        else if (elemental.type == ElementalType::Ice)
        {
            if (elemental.status == ElementalStatus::Grabbed)
            {
                DrawTexture(world->iceElementalCaptiveTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            }
            else
            {
                DrawTexture(world->iceElementalTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            }
        }
        else if (elemental.type == ElementalType::Spring)
        {
            BeginShaderMode(*entitiesShader);
            Vector4 tintVector = {
                1, 1, 1, 1};
            SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
            DrawTexture(world->springStaffTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            EndShaderMode();
        }
        else if (elemental.type == ElementalType::FireStaff)
        {
            BeginShaderMode(*entitiesShader);
            Vector4 tintVector = {
                1, 1, 1, 1};
            SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
            DrawTexture(world->fireStaffTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            EndShaderMode();
        }
        else if (elemental.type == ElementalType::IceStaff)
        {
            BeginShaderMode(*entitiesShader);
            Vector4 tintVector = {
                1, 1, 1, 1};
            SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
            DrawTexture(world->iceStaffTexture, elemental.position.x - TILE_SIZE / 2, elemental.position.y - TILE_SIZE, WHITE);
            EndShaderMode();
        }
    }

    world->particleSystem.Draw();

    // Draw a health bar for the player
    if (world->player.mortalEntity.health != world->player.mortalEntity.initialHealth)
    {
        DrawRectangle(world->player.position.x - 12, world->player.position.y + TILE_SIZE * 1.2f - 2, 54, 10, BLACK);
        DrawRectangle(world->player.position.x - 10, world->player.position.y + TILE_SIZE * 1.2f, 50, 6, RED);
        DrawRectangle(world->player.position.x - 10, world->player.position.y + TILE_SIZE * 1.2f, 50 * world->player.mortalEntity.health / world->player.mortalEntity.initialHealth, 6, GREEN);
    }

#ifdef _DEBUG
    Vector2 playerTilePos = GetTilePosition(world->player.position);
    DrawRectangle(playerTilePos.x * TILE_SIZE, playerTilePos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
    DrawRectangle(GetPlayerCenter(world).x - 2, GetPlayerCenter(world).y - 2, 4, 4, RED);
#endif

    for (const auto &tutorial : world->tutorialTexts)
    {
        if (tutorial.isUi)
            continue;
        DrawRichText(tutorial.text.c_str(), static_cast<int>(tutorial.position.x), static_cast<int>(tutorial.position.y), 20, WHITE);
    }

    RenderGrabbingStaff(world, entitiesShader);

    FXManager::DrawEffectsInWorld();

    EndMode2D();

    for (const auto &tutorial : world->tutorialTexts)
    {
        if (!tutorial.isUi)
            continue;
        DrawRichText(tutorial.text.c_str(), static_cast<int>(tutorial.position.x), static_cast<int>(tutorial.position.y), 20, WHITE);
    }

    FXManager::Draw();
}

void RenderGrabbingStaff(World *world, Shader *entitiesShader)
{
    if (!world->grabbingFireStaff && !world->grabbingIceStaff)
        return;

    Vector2 mousePosition = GetMousePosition();
    Vector2 worldMousePos = GetScreenToWorld2D(mousePosition, world->camera);

    Texture2D *gem = nullptr;
    if (world->grabbingFireStaff)
    {
        gem = &world->fireGemTexture;
        for (auto &elemental : world->elementals)
        {
            if (elemental.type == ElementalType::Fire)
            {
                for (int i = 0; i < 3; ++i)
                {
                    if (GetRandomFloat(0.0f, 1.0f) > 0.5f)
                    {
                        Color subtleWhite = Fade(RED, 0.5f);
                        world->particleSystem.Emit(worldMousePos, Vector2Subtract(elemental.position, worldMousePos), 5.0f, subtleWhite, 1.0f);
                    }
                }
                elemental.ChoosenPosition = worldMousePos;
            }
        }
    }
    else if (world->grabbingIceStaff)
    {
        gem = &world->iceGemTexture;
        for (auto &elemental : world->elementals)
        {
            if (elemental.type == ElementalType::Ice)
            {
                for (int i = 0; i < 3; ++i)
                {
                    if (GetRandomFloat(0.0f, 1.0f) > 0.5f)
                    {
                        Color subtleWhite = Fade(WHITE, 0.5f);
                        world->particleSystem.Emit(worldMousePos, Vector2Subtract(elemental.position, worldMousePos), 5.0f, subtleWhite, 1.0f);
                    }
                }
                elemental.ChoosenPosition = worldMousePos;
            }
        }
    }

    BeginShaderMode(*entitiesShader);
    Vector4 tintVector = {1, 1, 1, 1};
    SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
    auto pos = Vector2{worldMousePos.x - 7, worldMousePos.y - 7};
    DrawTexture(*gem, pos.x, pos.y, WHITE);
    world->gemPosition = pos;
    EndShaderMode();
}

void EmitParticlesFromElementals(float deltaTime, World *world)
{
    static float particleTimer = 0.0f;
    particleTimer += deltaTime;

    if (particleTimer >= 0.3f)
    {
        particleTimer = 0.0f;

        for (auto &elemental : world->elementals)
        {
            if (elemental.type == ElementalType::None ||
                elemental.type == ElementalType::FireStaff ||
                elemental.type == ElementalType::IceStaff ||
                elemental.status == ElementalStatus::Grabbed)
                continue;
            auto randomValue = GetRandomFloat(0.0f, 1.0f);
            if (randomValue > 0.5f)
                continue;

            int minX = std::max(0, static_cast<int>(elemental.position.x / TILE_SIZE) - world->elementalRange);
            int maxX = std::min(world->width - 1, static_cast<int>(elemental.position.x / TILE_SIZE) + world->elementalRange);
            int minY = std::max(0, static_cast<int>(elemental.position.y / TILE_SIZE) - world->elementalRange);
            int maxY = std::min(world->height - 1, static_cast<int>(elemental.position.y / TILE_SIZE) + world->elementalRange);

            for (int y = minY; y <= maxY; ++y)
            {
                for (int x = minX; x <= maxX; ++x)
                {

                    randomValue = GetRandomFloat(0.0f, 1.0f);
                    if (randomValue > 0.5f)
                        continue;
                    Vector2 targetPos = {x * TILE_SIZE + TILE_SIZE / 2.0f, y * TILE_SIZE + TILE_SIZE / 2.0f};
                    Vector2 velocity = Vector2Subtract(targetPos, elemental.position);
                    velocity = Vector2Scale(Vector2Normalize(velocity), 50.0f * randomValue);
                    Color color = (elemental.type == ElementalType::Fire) ? RED : (elemental.type == ElementalType::Ice) ? WHITE
                                                                                                                         : GREEN;
                    color.a = 255 * randomValue;
                    world->particleSystem.Emit(elemental.position, velocity, 5.0f, color, 1.0f);
                }
            }
        }
    }
}

void UpdateWorldState(World *world, float deltaTime)
{
    for (const auto &elemental : world->elementals)
    {
        if (elemental.type == ElementalType::None)
            continue;
        if (elemental.status == ElementalStatus::Grabbed)
            continue;

        Vector2 elementalTilePos = GetTilePosition(elemental.position);
        int minX = std::max(0, static_cast<int>(elementalTilePos.x) - world->elementalRange);
        int maxX = std::min(world->width, static_cast<int>(elementalTilePos.x) + world->elementalRange + 1);
        int minY = std::max(0, static_cast<int>(elementalTilePos.y) - world->elementalRange);
        int maxY = std::min(world->height, static_cast<int>(elementalTilePos.y) + world->elementalRange + 1);

        for (int y = minY; y < maxY; ++y)
        {
            for (int x = minX; x < maxX; ++x)
            {
                float distance = Vector2Distance(elementalTilePos, Vector2{static_cast<float>(x), static_cast<float>(y)});
                float influence = world->elementalRange - distance;
                if (influence < 0)
                    continue;
                influence = influence * influence / (world->elementalRange * world->elementalRange) * world->elementalPower;

                float current = world->tileStates[y][x];
                TileType tileType = world->tileTypes[y][x];
                float targetState;
                float rangeDelta;

                // The spring status is more stable and doesn't change as much
                if (elemental.type != ElementalType::Spring && tileType == TileType::Grass)
                {
                    influence *= 0.3f;
                }

                // Spring elementals can counteract the other elementals
                if (elemental.type == ElementalType::Spring)
                {
                    influence *= 3.0f;
                }

                if (elemental.type == ElementalType::Fire)
                {
                    targetState = dry_range.x;
                    rangeDelta = abs(current - dry_range.x);
                }
                else if (elemental.type == ElementalType::Ice)
                {
                    targetState = snow_range.y;
                    rangeDelta = abs(current - snow_range.y);
                }
                else if (elemental.type == ElementalType::Spring)
                {
                    targetState = grass_range.x + (grass_range.y - grass_range.x) / 2.0f;
                    rangeDelta = abs(current - targetState);
                }
                else
                {
                    continue;
                }

                if (world->tileTypes[y][x] != TileType::Block)
                {
                    float t = deltaTime * influence / (rangeDelta + 1e-6);
                    world->tileStates[y][x] = Lerp(current, targetState, t);
                }
            }
        }
    }

    EmitParticlesFromElementals(deltaTime, world);
    world->particleSystem.Update(deltaTime);
}

void UpdateTileStates(World *world, float deltaTime)
{
    int numGrassTiles = 0;
    for (int y = 0; y < world->height; y++)
    {
        for (int x = 0; x < world->width; x++)
        {
            float tileState = world->tileStates[y][x];
            TileType currentType = world->tileTypes[y][x];
            TileType newType = currentType;

            if (world->tileTypes[y][x] == TileType::Block)
            {
                numGrassTiles++;
                continue;
            }

            if (tileState <= dry_range.y)
            {
                world->tiles[y][x] = dry_color;
                newType = TileType::Dry;
            }
            else if (tileState <= grass_range.y)
            {
                world->tiles[y][x] = grass_color;
                newType = TileType::Grass;
            }
            else if (tileState <= snow_range.y)
            {
                world->tiles[y][x] = snow_color;
                newType = TileType::Snow;
            }

            if (newType == TileType::Grass)
            {
                numGrassTiles++;
            }

            if (newType != currentType)
            {
                world->tileTypes[y][x] = newType;
                if (world->firstTileComputed)
                {
                    NotifyStateChange(world, Rectangle{static_cast<float>(x), static_cast<float>(y), 1.0f, 1.0f}, currentType, newType);
                }
            }
        }
    }
    world->springDominance = static_cast<float>(numGrassTiles) / (world->width * world->height);
    world->springTiles = numGrassTiles;
    world->firstTileComputed = true;
}

bool IsCollidingWithBlocks(World *world, Vector2 proposedPosition)
{
    Rectangle playerRect = {proposedPosition.x - HALF_TILE_SIZE + 10, proposedPosition.y + 32 - HALF_TILE_SIZE, TILE_SIZE - 10, TILE_SIZE / 2 - 15};
    for (auto &block : world->blocks)
    {
        Rectangle blockRect = {block.position.x, block.position.y, TILE_SIZE / 2, TILE_SIZE};
        if (CheckCollisionRecs(playerRect, blockRect))
        {
            return true; // Collision detected
        }
    }
    return false; // No collision
}

void HandleInteractionWithElementals(World *world)
{

    auto playerCenter = GetPlayerCenter(world);
    if (world->player.status == PlayerStatus::Grabbing)
    {
        for (auto &elemental : world->elementals)
        {
            if (elemental.type != ElementalType::Fire &&
                elemental.type != ElementalType::Ice &&
                elemental.type != ElementalType::Spring &&
                elemental.type != ElementalType::FireStaff &&
                elemental.type != ElementalType::IceStaff)
                continue;

            if (elemental.status == ElementalStatus::Grabbed)
            {
                elemental.status = ElementalStatus::Moving;
                world->player.status = PlayerStatus::Moving;
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = elemental.position;

                elemental.position.x = playerCenter.x;
                elemental.position.y = playerCenter.y;

                if (elemental.type == ElementalType::FireStaff)
                {
                    world->grabbingFireStaff = false;
                }
                else if (elemental.type == ElementalType::IceStaff)
                {
                    world->grabbingIceStaff = false;
                }
                break;
            }
        }
        SoundManager::PlaySound(SFX_RELEASE, 0.3f, 0.1f);
    }

    else
    {

        float closestDistance = std::numeric_limits<float>::max();
        Elemental *closestElemental = nullptr;
        for (auto &elemental : world->elementals)
        {
            if (elemental.type != ElementalType::Fire &&
                elemental.type != ElementalType::Ice &&
                elemental.type != ElementalType::Spring &&
                elemental.type != ElementalType::FireStaff &&
                elemental.type != ElementalType::IceStaff)
                continue;

            float distance = Vector2Distance(playerCenter, elemental.position);
            if (distance < TILE_SIZE * 1.5f && elemental.status == ElementalStatus::Moving)
            {
                if (!closestElemental || distance < closestDistance)
                {
                    closestElemental = &elemental;
                    closestDistance = distance;
                }
            }
        }

        if (closestElemental)
        {
            closestElemental->status = ElementalStatus::Grabbed;
            world->player.status = PlayerStatus::Grabbing;
            SoundManager::PlaySound(SFX_GRAB, 0.3f, 0.1f);

            if (closestElemental->type == ElementalType::FireStaff)
            {
                world->grabbingFireStaff = true;
            }
            else if (closestElemental->type == ElementalType::IceStaff)
            {
                world->grabbingIceStaff = true;
            }
        }
    }
}

void UpdatePlayerHealth(World *world, float deltaTime)
{
    if (world->player.mortalEntity.isDead)
        return;

    world->player.mortalEntity.nextHealthCheck -= deltaTime;
    if (world->player.mortalEntity.nextHealthCheck <= 0.0f)
    {

        world->player.mortalEntity.nextHealthCheck = world->player.mortalEntity.checkRate;
        // if the player is on a non grass tile, decrease health
        // else increase health

        float lastHealth = world->player.mortalEntity.health;
        auto playerCenter = GetPlayerCenter(world);
        auto getPlayerTile = GetTilePosition(playerCenter);
        TileType currentTile = world->tileTypes[getPlayerTile.y][getPlayerTile.x];
        if (currentTile != TileType::Grass)
        {
            if (currentTile != TileType::Block)
            {
                world->player.mortalEntity.health -= world->player.mortalEntity.damageRate;
            }
        }
        else
        {
            world->player.mortalEntity.health += world->player.mortalEntity.damageRate;
        }

        if (world->player.mortalEntity.health <= 0.0f)
        {
            world->player.mortalEntity.isDead = true;
        }
        else if (world->player.mortalEntity.health > world->player.mortalEntity.initialHealth)
        {
            world->player.mortalEntity.health = world->player.mortalEntity.initialHealth;
        }

        if (lastHealth != world->player.mortalEntity.health)
        {
            NotifyPlayerHealthChange(world, lastHealth, world->player.mortalEntity.health);
        }
    }
}

void UpdatePlayer(World *world, float deltaTime)
{
    if (world->player.mortalEntity.isDead ||
        world->player.status == PlayerStatus::Idle)
        return;

    float directionX = 0.0f;
    float directionY = 0.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        directionX += 1.0f;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        directionX -= 1.0f;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        directionY += 1.0f;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        directionY -= 1.0f;
    }

    float length = sqrtf(directionX * directionX + directionY * directionY);
    if (length != 0.0f)
    {
        directionX /= length;
        directionY /= length;
    }

    float movementSpeed = world->player.speed * deltaTime;

    Vector2 newPositionX = {
        world->player.position.x + directionX * movementSpeed,
        world->player.position.y};

    Vector2 newPositionY = {
        world->player.position.x,
        world->player.position.y + directionY * movementSpeed};

    // Check X-axis movement for block collisions
    if (!IsCollidingWithBlocks(world, newPositionX))
    {
        world->player.position.x = newPositionX.x;
    }

    // Check Y-axis movement for block collisions
    if (!IsCollidingWithBlocks(world, newPositionY))
    {
        world->player.position.y = newPositionY.y;
    }

    // Ensure the player stays within the world bounds
    world->player.position.x = Clamp(world->player.position.x, 0.0f, (world->width - 1) * TILE_SIZE);
    world->player.position.y = Clamp(world->player.position.y, 0.0f, (world->height - 1) * TILE_SIZE);

    if (IsKeyReleased(KEY_SPACE))
    {
        HandleInteractionWithElementals(world);
    }

    UpdatePlayerHealth(world, deltaTime);
}

void UpdateCamera(World *world, float deltaTime)
{
    world->camera.target = world->player.position;
    world->camera.offset = Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    world->camera.rotation = 0.0f;
    world->camera.zoom = 1.0f;
}

void UpdateElementals(World *world, float deltaTime)
{
    for (auto &elemental : world->elementals)
    {
        if (elemental.type == ElementalType::None)
            continue;

        if (elemental.status == ElementalStatus::Grabbed)
        {
            elemental.position = world->player.position;
            continue;
        }

        if (world->grabbingIceStaff)
        {
            if (elemental.type == ElementalType::Ice)
            {
                // Go to the gem position
                auto moovement = Vector2Subtract(world->gemPosition, elemental.position);
                float distance = Vector2Length(moovement);
                elemental.position = Vector2Add(elemental.position, Vector2Scale(Vector2Normalize(moovement), std::min(elemental.speed * deltaTime, distance)));
                continue;
            }
        }
        else if (world->grabbingFireStaff)
        {
            if (elemental.type == ElementalType::Fire)
            {
                auto moovement = Vector2Subtract(world->gemPosition, elemental.position);
                float distance = Vector2Length(moovement);
                elemental.position = Vector2Add(elemental.position, Vector2Scale(Vector2Normalize(moovement), std::min(elemental.speed * deltaTime, distance)));
                continue;
            }
        }

        Vector2 direction = Vector2Subtract(elemental.ChoosenPosition, elemental.position);
        float distance = Vector2Length(direction);

        if (distance > 0)
        {
            Vector2 movement = Vector2Scale(Vector2Normalize(direction), std::min(elemental.speed * deltaTime, distance));
            elemental.position = Vector2Add(elemental.position, movement);
        }

        if (distance < elemental.speed * deltaTime)
        {

            float minX = std::max(0.0f, elemental.position.x - elemental.movementRadius * TILE_SIZE);
            float maxX = std::min((world->width - 1) * TILE_SIZE, elemental.position.x + elemental.movementRadius * TILE_SIZE);
            float minY = std::max(0.0f, elemental.position.y - elemental.movementRadius * TILE_SIZE);
            float maxY = std::min((world->height - 1) * TILE_SIZE, elemental.position.y + elemental.movementRadius * TILE_SIZE);

            elemental.ChoosenPosition = GetRandomVector(minX, minY, maxX, maxY);

            if (--elemental.timesUntilMovementIncrease <= 0)
            {
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.movementRadius++;
            }
        }
    }
}

void UpdateWorld(World *world, float deltaTime)
{
    FXManager::Update(deltaTime);

    if (VictoryCondition(world))
    {
        world->timeInVictory += deltaTime;
        return;
    }

    UpdatePlayer(world, deltaTime);
    UpdateWorldState(world, deltaTime);
    UpdateElementals(world, deltaTime);
    UpdateTileStates(world, deltaTime);
    UpdateCamera(world, deltaTime);
}

Vector2 GetTilePosition(const Vector2 &position)
{
    return Vector2{std::floor(position.x / TILE_SIZE), std::floor(position.y / TILE_SIZE)};
}

void DeleteWorld(World *world)
{
    if (!world)
        return;

    UnloadTexture(world->playerTexture);
    UnloadTexture(world->groundTexture);
    UnloadTexture(world->springStaffTexture);
    UnloadTexture(world->iceElementalTexture);
    UnloadTexture(world->iceElementalCaptiveTexture);
    UnloadTexture(world->fireElementalTexture);
    UnloadTexture(world->fireElementalCaptiveTexture);
    UnloadTexture(world->springStaffTexture);
    UnloadTexture(world->blockTexture);
    UnloadTexture(world->fireStaffTexture);
    UnloadTexture(world->iceStaffTexture);
    UnloadTexture(world->fireGemTexture);
    UnloadTexture(world->iceGemTexture);
    FXManager::Cleanup();
    delete world;
}

void NotifyStateChange(World *world, Rectangle where, TileType from, TileType to)
{
    where.x *= TILE_SIZE;
    where.y *= TILE_SIZE;
    where.width *= TILE_SIZE;
    where.height *= TILE_SIZE;
    FXManager::AddFadeRect(where, WHITE, 0.5f, true);

    switch (to)
    {
    case TileType::Dry:
        SoundManager::PlaySound(SFX_DRY, 0.3f, 0.1f);
        break;
    case TileType::Grass:
        SoundManager::PlaySound(SFX_GRASS, 0.1f, 0.5f);
        break;
    case TileType::Snow:
        SoundManager::PlaySound(SFX_FREEZE, 0.3f, 0.1f);
        break;
    case TileType::Block:
        break;
    default:
        break;
    }
}

void EmitParticlesAroundPlayer(World *w, Vector2 playerPosition, int numParticles, float particleRadius, Color color, float lifeTime, bool from)
{
    float angle, distance, speed;
    Vector2 emitPosition, velocity;

    for (int i = 0; i < numParticles; i++)
    {
        angle = GetRandomValue(0, 360) * DEG2RAD;
        distance = GetRandomValue(10, 50);

        emitPosition.x = playerPosition.x + cos(angle) * distance;
        emitPosition.y = playerPosition.y + sin(angle) * distance;

        speed = GetRandomValue(200, 400) / 100.0f;

        if (from)
        {
            velocity.x = (emitPosition.x - playerPosition.x) * speed;
            velocity.y = (emitPosition.y - playerPosition.y) * speed;
        }
        else
        {
            velocity.x = (playerPosition.x - emitPosition.x) * speed;
            velocity.y = (playerPosition.y - emitPosition.y) * speed;
        }

        w->particleSystem.Emit(emitPosition, velocity, particleRadius, color, lifeTime);
    }
}

void NotifyPlayerHealthChange(World *world, float lastHealth, float newHealth)
{
    Vector2 centeredPlayerPos = GetPlayerCenter(world);
    if (newHealth < lastHealth)
    {
        FXManager::AddFadeRect(Rectangle{0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, RED, 0.025f, false);
        EmitParticlesAroundPlayer(world, centeredPlayerPos, 10, 2.0f, RED, 0.7f, true);
        SoundManager::PlaySound(SFX_HIT, 0.3f, 0.1f);
    }
    else
    {
        // FXManager::AddFadeRect(Rectangle{0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, GREEN, 0.025f, false);
        EmitParticlesAroundPlayer(world, centeredPlayerPos, 10, 3.0f, GREEN, 0.7f, false);
        SoundManager::PlaySound(SFX_HEAL, 0.3f, 0.1f);
    }
}

Vector2 GetPlayerCenter(World *world)
{
    return Vector2{world->player.position.x + HALF_TILE_SIZE, world->player.position.y + HALF_TILE_SIZE};
}

bool VictoryCondition(World *world)
{
    bool victory = world->springDominance >= 1.0f;
    if(!world->wasInVictory)
    {
        if(victory)
        {
            SoundManager::PlaySound(SFX_VICTORY, 0.3f, 0.1f);
            world->wasInVictory = true;
            FXManager::AddFadeRect(Rectangle{0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, GREEN, 0.025f, false);
        }
    }
    return victory;
}