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

    float richTextSize = MeasureText("Press [Enter] to summon the spring guardian", 20);
    DrawRichText("Press <color=0,255,155,255> [Enter] </color> to summon the spring guardian", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2, 20, WHITE);

    EnableVolumeOptions(true);
}

void InGameScene::UpdateStarting(float delta)
{
    if (IsKeyReleased(KEY_ENTER))
    {
        gameState = GameState::PLAYING;
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
    SetMusicVolume(music, fmax(world->springDominance * 0.4f, 0.1f));

    if (world->springDominance >= 1.0f)
    {
        gameState = GameState::VICTORY;
        SoundManager::PlaySound(SFX_VICTORY, 0.5f, 0.1f);
    }
    else if (world->player.mortalEntity.isDead)
    {
        gameState = GameState::GAME_OVER;
    }

    if(IsKeyDown(KEY_R))
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

    richTextSize = MeasureText("Press [M] to return to the main menu", 20);
    DrawRichText("Press <color=150,0,0,255> [M] </color> to return to the main menu", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 120, 20, WHITE);


    EnableVolumeOptions(true);
}

void InGameScene::UpdateGameOver(float deltaTime)
{
    if (IsKeyReleased(KEY_R))
    {
        DeleteWorld(world);
        world = GetWorld(currentLevel);
        gameState = GameState::PLAYING;
    }

    if (IsKeyReleased(KEY_M))
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

    float victoryTextSize = MeasureText("Victory!", 40);
    DrawRichText("<color=0,255,155,255>Victory!</color>", SCREEN_WIDTH / 2 - victoryTextSize / 2, 150, 40, WHITE);

    float explaationSize = MeasureText("Spring HAS come!", 25);
    DrawRichText("Spring <color=0,255,155,255>HAS</color> come!", SCREEN_WIDTH / 2 - explaationSize / 2, 220, 25, WHITE);

    float richTextSize = 0;

    // Show next world message
    if (currentLevel < registeredWorlds.size())
    {
        richTextSize = MeasureText("Press [N] to continue to the next level", 20);
        DrawRichText("Press <color=0,255,155,255> [N] </color> to continue to the next level", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);
    }
    else
    {
        richTextSize = MeasureText("Congratulations you finsihed the game!!", 20);
        DrawRichText("Congratulations you <color=0,255,155,255>finsihed</color> the game!!", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);

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
    currentLevel = 6;

    RegisterWorld(1);
    RegisterWorld(2);
    RegisterWorld(3);
    RegisterWorld(4);
    RegisterWorld(5);
    RegisterWorld(6);

    this->world = GetWorld(currentLevel);

    distortionShader = LoadDistorionShader();
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(distortionShader, GetShaderLocation(distortionShader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

    entitiesShader = LoadEntitiesShader();
    SetShaderValue(entitiesShader, GetShaderLocation(entitiesShader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

    music = LoadMusicStream("resources/in_game_music.mp3");
    PlayMusicStream(music);

    background = LoadTexture("resources/splash.png");
}

void InGameScene::Update(float deltaTime)
{
    UpdateMusicStream(music);
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
    UnloadMusicStream(music);
}
