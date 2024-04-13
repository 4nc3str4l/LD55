#include "raylib.h"
#include "world.h"

auto SCREEN_WIDTH = 1024;
auto SCREEN_HEIGHT = 576;
auto TITLE = "Ludum Dare 55";

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    InitAudioDevice();

    Image image = LoadImage("resources/BasicElemental.png");
    Texture2D basicElementalTexture = LoadTextureFromImage(image);
    UnloadImage(image);

    Sound sound = LoadSound("resources/random.wav");

    SetTargetFPS(60);

    World world = LoadWorld("resources/worlds/level_1_ground.csv", "resources/worlds/level_1_monsters.csv");

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        BeginDrawing();
        ClearBackground(GRAY);

        UpdateWorld(world, deltaTime);
        RenderWorld(world);
        DrawTexture(basicElementalTexture,
                    world.player.position.x,
                    world.player.position.y,
                    GREEN);

        DrawText(TITLE, 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadSound(sound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
