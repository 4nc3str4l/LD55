#include "InGameScene.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "constants.h"
#include "raylib.h"
#include "world.h"
#include "utils.h"

InGameScene::InGameScene() : GameScene("In-Game Scene") {}


void DrawFormattedText(int springTiles, int totalTiles, float percentage, int posX, int posY) {
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
    DrawRichText("Press <color=0,255,0,255> [Enter] </color> to summon the spring guardian", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2, 20, WHITE);
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
    DrawFormattedText(world->springTiles, world->width* world->height,
                            world->springDominance, 225, 15);
    // Print the level where we are
    DrawText(FormatText("Level: %i", world->currentLevel).c_str(), SCREEN_WIDTH - 100, 15, 20, WHITE);
}

void InGameScene::UpdatePlaying(float deltaTime)
{
    SetShaderValue(entitiesShader, GetShaderLocation(entitiesShader, "time"), &timeElapsed, SHADER_UNIFORM_FLOAT);
    UpdateWorld(world, deltaTime);
    SetMusicVolume(music, world->springDominance * 0.4f);

    if(world->springDominance >= 1.0f) {
        gameState = GameState::VICTORY;
    }
}

void InGameScene::DrawPlaying(World *world)
{
    RenderWorld(world, &distortionShader, &entitiesShader);
    DrawInGameUI(world);
}

void InGameScene::DrawGameOverUI() 
{
    float victoryTextSize = MeasureText("GameOver!", 40);
    DrawText("Game Over!", SCREEN_WIDTH / 2 - victoryTextSize / 2, SCREEN_HEIGHT / 2 - 40, 40, WHITE);


    float richTextSize = MeasureText("Press [Enter] to continue", 20);
    DrawRichText("Press <color=0,255,0,255> [Enter] </color> to continue", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2, 20, WHITE);
}

void InGameScene::UpdateGameOver(float deltaTime) 
{
    if (IsKeyReleased(KEY_ENTER))
    {
        gameState = GameState::IN_MENU;
    }
}


void InGameScene::DrawVictoryUI() 
{
    float victoryTextSize = MeasureText("Victory!", 40);
    DrawText("Victory!", SCREEN_WIDTH / 2 - victoryTextSize / 2, SCREEN_HEIGHT / 2 - 40, 40, WHITE);


    float richTextSize = MeasureText("Press [Enter] to continue", 20);
    DrawRichText("Press <color=0,255,0,255> [Enter] </color> to continue", SCREEN_WIDTH / 2 - richTextSize / 2, SCREEN_HEIGHT / 2, 20, WHITE);
}

void InGameScene::UpdateVictory(float deltaTime) 
{
    if (IsKeyReleased(KEY_ENTER))
    {
        gameState = GameState::IN_MENU;
    }
}


void InGameScene::UpdateInMenuUI(float deltaTime) 
{

}

void InGameScene::DrawInMenuUI(World* world) 
{
    DrawInGameUI(world);
}

World* InGameScene::GetWorld(int level) {
    for (auto &w : registeredWorlds) {
        if (w.level == level) {
            std::string levelStr = std::to_string(level);
            std::string worldPath = "resources/worlds/level_" + levelStr + "_ground.csv";
            std::string entitiesPath = "resources/worlds/level_" + levelStr + "_entities.csv";
            std::string tutorialPath = "resources/worlds/level_" + levelStr + "_tutorial.txt";
            return LoadWorld(worldPath, entitiesPath, tutorialPath);
        }
    }
    return nullptr;
}

void InGameScene::Load() 
{
    RegisterWorld(1);

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

void InGameScene::Update(float deltaTime) {
    UpdateMusicStream(music);
    timeElapsed += deltaTime;
    if(timeElapsed > 10000.0f) 
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

void InGameScene::RegisterWorld(int level) {
    registeredWorlds.push_back({level});
}

void InGameScene::Unload() {
    DeleteWorld(world);
    UnloadShader(distortionShader);
    UnloadShader(entitiesShader);
    UnloadMusicStream(music);
}
