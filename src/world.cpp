#include "world.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "raymath.h"
#include <algorithm>
#include "utils.h"
#include <limits>

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

World *LoadWorld(const std::string &worldPath,
                 const std::string &entitiesPath,
                 const std::string &tutorialPath)
{
    auto world = new World(); // Creando dinámicamente un nuevo World

    int width = 0;
    int height = 0;

    auto data = LoadDataMatrix(worldPath, width, height);
    auto entities = LoadDataMatrix(entitiesPath, width, height);
    // Load tutorials if the file exists
    world->tutorialTexts = LoadTutorialText(tutorialPath);
    world->width = width;
    world->height = height;

    world->elementals.resize(width * height);
    world->tiles.resize(height, std::vector<Color>(width));
    world->tileTypes.resize(height, std::vector<TileType>(width));
    world->tileStates.resize(height, std::vector<float>(width));

    world->playerTexture = GetTextureFromPath(player_texture_path);
    world->groundTexture = GetTextureFromPath(ground_texture_path);

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
                world->tileTypes[y][x] = TileType::Grass;
                world->tileStates[y][x] = 1.0f;
                break;
            case 3:
                world->blocks.push_back({Vector2{x * TILE_SIZE, y * TILE_SIZE}});
                world->tileTypes[y][x] = TileType::Block;
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
                world->elementals[y * world->width + x] = elemental;
            }
            else if (entity == 3)
            {
                auto elemental = Elemental{position, ElementalType::Ice};
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                world->elementals[y * world->width + x] = elemental;
            }
            else if (entity == 4)
            {
                auto elemental = Elemental{position, ElementalType::Spring};
                elemental.movementRadius = 0;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = position;
                elemental.speed = 0.0f;
                world->elementals[y * world->width + x] = elemental;
            }
        }
    }

    return world;
}

void RenderPhysics(const World *world)
{
    // Render the player's collision rectangle
    Rectangle playerRect = {
        world->player.position.x - HALF_TILE_SIZE,
        world->player.position.y + 32 - HALF_TILE_SIZE,
        TILE_SIZE,
        TILE_SIZE / 2};
    DrawRectangleRec(playerRect, Fade(PURPLE, 0.5)); // Semi-transparent purple for visibility

    // Render blocks' collision rectangles
    for (const auto &block : world->blocks)
    {
        Rectangle blockRect = {block.position.x, block.position.y, TILE_SIZE, TILE_SIZE};
        DrawRectangleRec(blockRect, Fade(ORANGE, 0.5)); // Semi-transparent orange for visibility
    }
}

void RenderWorld(World *world, Shader *distortionShader, Shader *entitiesShader)
{
    BeginMode2D(world->camera);
    for (int y = 0; y < world->height; y++)
    {
        for (int x = 0; x < world->width; x++)
        {
            // Calcula el ángulo de rotación basado en la posición
            float rotation = ((x + y) % 4) * 90; // Esto rota las texturas en 0, 90, 180, o 270 grados

            // Dibuja la textura con rotación
            Rectangle sourceRec = {0, 0, TILE_SIZE, TILE_SIZE};                                                             // Región de la textura a dibujar
            Rectangle destRec = {x * TILE_SIZE + TILE_SIZE / 2.0f, y * TILE_SIZE + TILE_SIZE / 2.0f, TILE_SIZE, TILE_SIZE}; // Destino y tamaño en pantalla
            Vector2 originRec = {TILE_SIZE / 2.0f, TILE_SIZE / 2.0f};                                                       // Punto de origen para rotar

            // Ajusta las posiciones y rota la textura
            DrawTexturePro(world->groundTexture, sourceRec, destRec, originRec, rotation, world->tiles[y][x]);
        }
    }
    for (int i = 0; i < world->width * world->height; i++)
    {
        auto elemental = world->elementals[i];
        if (elemental.type == ElementalType::Fire)
        {
            DrawRectangle(elemental.position.x - HALF_TILE_SIZE, elemental.position.y - HALF_TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
        }
        else if (elemental.type == ElementalType::Ice)
        {
            DrawRectangle(elemental.position.x - HALF_TILE_SIZE, elemental.position.y - HALF_TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
        }
        else if (elemental.type == ElementalType::Spring)
        {
            DrawRectangle(elemental.position.x - HALF_TILE_SIZE, elemental.position.y - HALF_TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
        }
    }

    for (const auto &block : world->blocks)
    {
        DrawRectangle(block.position.x, block.position.y, TILE_SIZE, TILE_SIZE, GRAY);
    }

    world->particleSystem.Draw();

    BeginShaderMode(*entitiesShader);

    Vector4 tintVector = {
        GREEN.r / 255.0f,
        GREEN.g / 255.0f,
        GREEN.b / 255.0f,
        fmax(world->springDominance, 0.2f),
    };

    SetShaderValue(*entitiesShader, GetShaderLocation(*entitiesShader, "tint"), &tintVector, SHADER_UNIFORM_VEC4);
    DrawTexture(world->playerTexture,
                world->player.position.x,
                world->player.position.y - TILE_SIZE,
                GREEN);
    EndShaderMode();

#ifdef _DEBUG
    Vector2 playerTilePos = GetTilePosition(world->player.position);
    DrawRectangle(playerTilePos.x * TILE_SIZE, playerTilePos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
#endif

    for (const auto &tutorial : world->tutorialTexts)
    {
        if (tutorial.isUi)
            continue;
        DrawRichText(tutorial.text.c_str(), static_cast<int>(tutorial.position.x), static_cast<int>(tutorial.position.y), 20, WHITE);
    }

#ifdef _DEBUG
    RenderPhysics(world);
#endif

    EndMode2D();

    for (const auto &tutorial : world->tutorialTexts)
    {
        if (!tutorial.isUi)
            continue;
        DrawRichText(tutorial.text.c_str(), static_cast<int>(tutorial.position.x), static_cast<int>(tutorial.position.y), 20, WHITE);
    }
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
            if (elemental.type == ElementalType::None || elemental.status == ElementalStatus::Grabbed)
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
                    Color color = (elemental.type == ElementalType::Fire) ? RED : (elemental.type == ElementalType::Ice) ? BLUE
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

                float t = deltaTime * influence / (rangeDelta + 1e-6);
                world->tileStates[y][x] = Lerp(current, targetState, t);
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
            if (tileState <= dry_range.y)
            {
                world->tiles[y][x] = dry_color;
                world->tileTypes[y][x] = TileType::Dry;
            }
            else if (tileState <= grass_range.y)
            {
                world->tiles[y][x] = grass_color;
                world->tileTypes[y][x] = TileType::Grass;
            }
            else if (tileState <= snow_range.y)
            {
                world->tiles[y][x] = snow_color;
                world->tileTypes[y][x] = TileType::Snow;
            }

            // Blocks count as spring for simplicity
            if (world->tileTypes[y][x] == TileType::Grass ||
                world->tileTypes[y][x] == TileType::Block)
            {
                numGrassTiles++;
            }
        }
    }
    world->springDominance = static_cast<float>(numGrassTiles) / (world->width * world->height);
    world->springTiles = numGrassTiles;
}

bool IsCollidingWithBlocks(World *world, Vector2 proposedPosition)
{
    Rectangle playerRect = {proposedPosition.x - HALF_TILE_SIZE, proposedPosition.y + 32 - HALF_TILE_SIZE, TILE_SIZE, TILE_SIZE / 2};
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

    if (world->player.status == PlayerStatus::Grabbing)
    {
        for (auto &elemental : world->elementals)
        {
            if (elemental.type != ElementalType::Fire &&
                elemental.type != ElementalType::Ice &&
                elemental.type != ElementalType::Spring)
                continue;

            if (elemental.status == ElementalStatus::Grabbed)
            {
                elemental.status = ElementalStatus::Moving;
                world->player.status = PlayerStatus::Moving;
                elemental.movementRadius = 1;
                elemental.timesUntilMovementIncrease = TIMES_INTIL_MOVEMENT_RADIUS_INCRESES;
                elemental.ChoosenPosition = elemental.position;
                break;
            }
        }
    }

    else
    {
        float closestDistance = std::numeric_limits<float>::max();
        Elemental *closestElemental = nullptr;
        for (auto &elemental : world->elementals)
        {
            if (elemental.type != ElementalType::Fire &&
                elemental.type != ElementalType::Ice &&
                elemental.type != ElementalType::Spring)
                continue;
            float distance = Vector2Distance(world->player.position, elemental.position);
            if (distance < TILE_SIZE * 2.5 && elemental.status == ElementalStatus::Moving)
            {
                if(!closestElemental || distance < closestDistance)
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
        }
    }
}

void UpdatePlayer(World *world, float deltaTime)
{
    if (world->player.status == PlayerStatus::Dead ||
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
}