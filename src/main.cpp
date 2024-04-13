#include "raylib.h"
#include "world.h"

auto SCREEN_WIDTH = 1024;
auto SCREEN_HEIGHT = 576;
auto TITLE = "Ludum Dare 55";

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    InitAudioDevice();

    Sound sound = LoadSound("resources/random.wav");

    SetTargetFPS(60);

    World world = LoadWorld("resources/worlds/level_1_ground.csv",
                    "resources/worlds/level_1_entities.csv");

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        BeginDrawing();
        ClearBackground(GRAY);

        UpdateWorld(world, deltaTime);
        RenderWorld(world);

        DrawText(TITLE, 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadSound(sound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

