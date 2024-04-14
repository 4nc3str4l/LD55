#include "InGameScene.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "constants.h"
#include "raylib.h"
#include "world.h"
#include "utils.h"
#include "SceneManager.h"
#include "SoundManager.h"

InGameScene::InGameScene() : GameScene("InGameScene") {}

void DrawFormattedText(int springTiles, int totalTiles, float percentage, int posX, int posY)
{
    std::ostringstream ss;

    // Build the formatted string with fixed-point notation and two decimal places
    ss << "Spring Tiles: " << springTiles << " / " << totalTiles
       << " (" << std::fixed << std::setprecision(2) << percentage << "%)";

    // Draw the text on the screen using Raylib
    DrawText(ss.str().c_str(), posX, posY, 4, WHITE);
}

void InGameScene::DrawStartingUI()
{
    DrawTexture(background, 0, 0, WHITE);
    BeginShaderMode(distortionShader);
    DrawTexture(background, 0, 0, WHITE);
    EndShaderMode();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f));

    // Render the level in big
    // Get the color 0, 255, 155, 255
    float levelTextSize = MeasureText(FormatText("Level %i", world->currentLevel).c_str(), 40);
    DrawRichText(FormatText("Level %i", world->currentLevel).c_str(), SCREEN_WIDTH / 2 - levelTextSize / 2, 150, 40, WHITE);

    auto color = Color{0, 255, 155, 255};
    auto name = GetLevelName(world->currentLevel);
    float levelNameSize = MeasureText(name.c_str(), 15);
    DrawRichText(name.c_str(), SCREEN_WIDTH / 2 - levelNameSize / 2, 200, 15, color);

    float richTextSize = MeasureText("Press [Enter] to start the game", 22);
    DrawRichText("Press <color=0,255,155,255> [Enter] </color> to start the game", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 10, 22, WHITE);

    if (currentLevel < registeredWorlds.size())
    {
        richTextSize = MeasureText("Press [N] to jump to the next level", 20);
        DrawRichText("Press <color=0,255,155,255> [N] </color> to jump to the next level", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 110, 20, WHITE);
    }

    if (currentLevel > 1)
    {
        richTextSize = MeasureText("Press [L] for previous level", 20);
        DrawRichText("Press <color=0,255,155,255> [L] </color> for previous level", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 150, 20, WHITE);
    }

    DrawRichText("Press <color=150,0,0,255> [M] </color> Main Menu", 10, SCREEN_HEIGHT - 40, 20, WHITE);

    EnableVolumeOptions(true);
}

void InGameScene::UpdateStarting(float delta)
{
    if (IsKeyDown(KEY_ENTER))
    {
        gameState = GameState::PLAYING;
        SoundManager::PlaySound(SFX_GRASS, 0.5f, 0.1f);
        SoundManager::PlayMusic(SoundManager::gameMusic, 0.2f);
    }

    if (IsKeyDown(KEY_M))
    {
        SceneManager::GetInstance().ChangeScene("Splash");
    }

    if (IsKeyReleased(KEY_N) && currentLevel < registeredWorlds.size())
    {
        currentLevel++;
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::STARTING;
        SoundManager::PlaySound(SFX_GRASS, 0.5f, 0.1f);
    }

    if (IsKeyReleased(KEY_L) && currentLevel > 1)
    {
        currentLevel--;
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::STARTING;

        SoundManager::PlaySound(SFX_GRASS, 0.5f, 0.1f);
    }
}

void InGameScene::DrawInGameUI(const World *world)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, 40, BLACK);
    DrawText("Spring Dominance:", 10, 10, 20, WHITE);
    DrawRectangle(200, 15, 200, 10, RED);
    DrawRectangle(200, 15, 200 * world->springDominance, 10, LIME);
    // Draw total number of tiles / number of missing tiles
    DrawFormattedText(world->springTiles, world->width * world->height,
                      world->springDominance, 225, 15);
    // Print the level where we are
    DrawText(FormatText("Level: %i", world->currentLevel).c_str(), SCREEN_WIDTH - 100, 15, 20, WHITE);

    // Show the surender option on the top bar
    DrawRichText("Press <color=200,0,0,255>[R]</color> to restart", SCREEN_WIDTH - 400, 15, 20, WHITE);
}

void InGameScene::UpdatePlaying(float deltaTime)
{
    SetShaderValue(entitiesShader, GetShaderLocation(entitiesShader, "time"), &timeElapsed, SHADER_UNIFORM_FLOAT);
    UpdateWorld(world, deltaTime);

    if (VictoryCondition(world) && world->timeInVictory > 1.0f)
    {
        gameState = GameState::VICTORY;
        SoundManager::PlayMusic(SoundManager::titleMusic, 0.7f);
    }
    else if (world->player.mortalEntity.isDead)
    {
        gameState = GameState::GAME_OVER;
        SoundManager::PlaySound(SFX_HIT, 0.5f, 0.1f);
        SoundManager::StopMusic();
        SoundManager::PlayMusic(SoundManager::titleMusic, 0.1f);
    }

    if (IsKeyDown(KEY_R))
    {
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::STARTING;
    }

    EnableVolumeOptions(false);
}

void InGameScene::DrawPlaying(World *world)
{
    RenderWorld(world, &distortionShader, &entitiesShader);
    DrawInGameUI(world);
}

void InGameScene::DrawGameOverUI()
{

    DrawTexture(background, 0, 0, WHITE);
    BeginShaderMode(distortionShader);
    DrawTexture(background, 0, 0, WHITE);
    EndShaderMode();

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f));
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(RED, 0.2f));

    float victoryTextSize = MeasureText("Game Over!", 40);
    DrawRichText("<color=255,0,0,255>Game Over!</color>", SCREEN_WIDTH / 2 - victoryTextSize / 2, 150, 40, WHITE);

    float richTextSize = MeasureText("Spring won't come this year...", 20);

    richTextSize = MeasureText("Press [R] if it is NOT OVER yet!", 20);
    DrawRichText("Press <color=0,255,155,255> [R] </color> if it is <color=0,255,155,255>NOT OVER</color> yet!", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);

    DrawRichText("Press <color=150,0,0,255> [M] </color> Main Menu", 10, SCREEN_HEIGHT - 40, 20, WHITE);

    EnableVolumeOptions(true);
}

void InGameScene::UpdateGameOver(float deltaTime)
{
    if (IsKeyDown(KEY_R))
    {
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::PLAYING;
    }

    if (IsKeyDown(KEY_M))
    {
        SceneManager::GetInstance().ChangeScene("Splash");
    }
}

void InGameScene::DrawVictoryUI()
{
    DrawTexture(background, 0, 0, WHITE);
    BeginShaderMode(distortionShader);
    DrawTexture(background, 0, 0, WHITE);
    EndShaderMode();

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f));
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(GREEN, 0.1f));

    bool gameComplete = currentLevel >= registeredWorlds.size();

    if (!gameComplete)
    {
        float victoryTextSize = MeasureText("Victory!", 40);
        DrawRichText("<color=0,255,155,255>Victory!</color>", SCREEN_WIDTH / 2 - victoryTextSize / 2, 150, 40, WHITE);
        float explaationSize = MeasureText("Spring HAS come!", 25);
        DrawRichText("Spring <color=0,255,155,255>HAS</color> come!", SCREEN_WIDTH / 2 - explaationSize / 2, 220, 25, WHITE);
    }
    else
    {
        float explaationSize = MeasureText("Spring HAS come... FOREVER!", 25);
        DrawRichText("Spring <color=0,255,155,255>HAS</color> come... <color=0,255,155,255>FOREVER</color>", SCREEN_WIDTH / 2 - explaationSize / 2, 150, 25, WHITE);
    }

    float richTextSize = 0;

    // Show next world message
    if (!gameComplete)
    {
        richTextSize = MeasureText("Press [N] to continue to the next level", 20);
        DrawRichText("Press <color=0,255,155,255> [N] </color> to continue to the next level", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);
    }
    else
    {
        richTextSize = MeasureText("Congratulations you FINISHED the game!!", 20);
        DrawRichText("Congratulations you <color=0,255,155,255>FINISHED</color> the game!!", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 - 60, 20, WHITE);

        richTextSize = MeasureText("Thanks a lot for playing, it means the WORLD to ME...", 20);
        DrawRichText("Thanks a lot for playing, it means the <color=0, 255, 155, 255> WORLD</color> to<color=0,255,155,255> ME</color>...", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2, 20, WHITE);

        richTextSize = MeasureText("Press [M] to return to the main menu", 20);
        DrawRichText("Press <color=150,0,0,255> [M] </color> to return to the main menu", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 120, 20, WHITE);
    }

    // Show repeat level message
    richTextSize = MeasureText("Press [R] to repeat the level", 20);
    DrawRichText("Press <color=0,255,155,255> [R] </color> to repeat the level", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 150, 20, WHITE);

    EnableVolumeOptions(true);
}

void InGameScene::UpdateVictory(float deltaTime)
{
    if (IsKeyDown(KEY_N) && currentLevel < registeredWorlds.size())
    {
        currentLevel++;
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::PLAYING;
    }

    if (IsKeyDown(KEY_R))
    {
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::PLAYING;
    }

    if (IsKeyDown(KEY_M) && currentLevel >= registeredWorlds.size())
    {
        SceneManager::GetInstance().ChangeScene("Splash");
    }
}

void InGameScene::UpdateInMenuUI(float deltaTime)
{
}

void InGameScene::DrawInMenuUI(World *world)
{
    DrawInGameUI(world);
}

World *InGameScene::GetWorld(int level)
{
    for (auto &w : registeredWorlds)
    {
        if (w.level == level)
        {
            std::string levelStr = std::to_string(level);
            std::string worldPath = "resources/worlds/level_" + levelStr + "_ground.csv";
            std::string entitiesPath = "resources/worlds/level_" + levelStr + "_entities.csv";
            std::string tutorialPath = "resources/worlds/level_" + levelStr + "_tutorial.txt";
            return LoadWorld(level, worldPath, entitiesPath, tutorialPath);
        }
    }
    return nullptr;
}

void InGameScene::Load()
{
    gameState = GameState::STARTING;
    currentLevel = 1;

    RegisterWorld(1);
    RegisterWorld(2);
    RegisterWorld(3);
    RegisterWorld(4);
    RegisterWorld(5);
    RegisterWorld(6);
    RegisterWorld(7);
    RegisterWorld(8);
    RegisterWorld(9);
    RegisterWorld(10);
    RegisterWorld(11);

    this->world = GetWorld(currentLevel);

    distortionShader = LoadDistorionShader();
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(distortionShader, GetShaderLocation(distortionShader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

    entitiesShader = LoadEntitiesShader();
    SetShaderValue(entitiesShader, GetShaderLocation(entitiesShader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

    SoundManager::PlayMusic(SoundManager::gameMusic, 0.5f);

    background = LoadTexture("resources/splash.png");
}

void InGameScene::Update(float deltaTime)
{
    timeElapsed += deltaTime;
    if (timeElapsed > 10000.0f)
    {
        timeElapsed = 0.0f;
    }
    SetShaderValue(distortionShader, GetShaderLocation(distortionShader, "time"), &timeElapsed, SHADER_UNIFORM_FLOAT);

    switch (gameState)
    {
    case GameState::STARTING:
        UpdateStarting(deltaTime);
        break;
    case GameState::PLAYING:
        UpdatePlaying(deltaTime);
        break;
    case GameState::GAME_OVER:
        UpdateGameOver(deltaTime);
        break;
    case GameState::IN_MENU:
        UpdateInMenuUI(deltaTime);
        break;
    case GameState::VICTORY:
        UpdateVictory(deltaTime);
        break;
    }
}

void InGameScene::Render()
{
    switch (gameState)
    {
    case GameState::STARTING:
        DrawStartingUI();
        break;
    case GameState::PLAYING:
        DrawPlaying(world);
        break;
    case GameState::GAME_OVER:
        DrawGameOverUI();
        break;
    case GameState::IN_MENU:
        DrawInMenuUI(world);
        break;
    case GameState::VICTORY:
        DrawVictoryUI();
        break;
    }
}

void InGameScene::RegisterWorld(int level)
{
    registeredWorlds.push_back({level});
}

void InGameScene::Unload()
{
    registeredWorlds.clear();
    DeleteWorld(world);
    UnloadShader(distortionShader);
    UnloadShader(entitiesShader);
}

std::string InGameScene::GetLevelName(int level)
{
    switch (level)
    {
    case 1:
        return "Baby Steps";
    case 2:
        return "Growing is Hard";
    case 3:
        return "Making Friends";
    case 4:
        return "3 is a Crowd";
    case 5:
        return "Balance...";
    case 6:
        return "Some things cannot be fixed";
    case 7:
        return "That feeling of being trapped";
    case 8:
        return "Bigger problems";
    case 9:
        return "Remote control";
    case 10:
        return "Enemies/Allies who knows...";
    case 11:
        return "Back to the roots";
    default:
        return "Missing level name";
    }
}
