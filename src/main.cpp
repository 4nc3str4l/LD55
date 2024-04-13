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

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);

        for(int y = 0; y < WORLD_HEIGHT; y++)
        {
            for(int x = 0; x < WORLD_WIDTH; x++)
            {
                auto color = dry_color;
                if (y % 2 == 0)
                {
                    color = grass_color;
                }
                else
                {
                    color = snow_color;
                }
                DrawRectangle(x * 32, y * 32, 32, 32, color);
            }
        }

        DrawTexture(basicElementalTexture, 0, 0, GREEN);

        DrawText("Ludum Dare 55", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadSound(sound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
