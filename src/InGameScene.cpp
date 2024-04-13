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

void DrawUI(const World *world)
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


void InGameScene::Load() {
    World* w = LoadWorld("resources/worlds/level_1_ground.csv",
                    "resources/worlds/level_1_entities.csv",
                    "resources/worlds/level_1_tutorial.txt");
    this->world = w;
}

void InGameScene::Update(float deltaTime) {
    UpdateWorld(world, deltaTime);
}

void InGameScene::Render() {
    RenderWorld(world);
    DrawUI(world);
}

void InGameScene::Unload() {
    DeleteWorld(world);
}
