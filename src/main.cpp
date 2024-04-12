#include "raylib.h"


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Ludum Dare 55 ");
    InitAudioDevice();

    Rectangle rec = { 0.0f, 0.0f, 60.0f, 60.0f };
    Vector2 velocity = { 4.0f, 4.0f };

    Sound sound = LoadSound("resources/random.wav");

    Rectangle button = { 200.0f, 200.0f, 200.0f, 50.0f };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Ludum Dare 55", 10, 10, 20, DARKGRAY);

        rec.x += velocity.x;
        rec.y += velocity.y;

        if (rec.x >= (screenWidth - rec.width) || rec.x <= 0) velocity.x *= -1.0f;
        if (rec.y >= (screenHeight - rec.height) || rec.y <= 0) velocity.y *= -1.0f;

        DrawRectangleRec(rec, RED);

        if (CheckCollisionPointRec(GetMousePosition(), button))
        {
            DrawRectangleRec(button, LIGHTGRAY);
            DrawText("Click me!", button.x + 10, button.y + 10, 20, DARKGRAY);
        }
        else
        {
            DrawRectangleRec(button, GRAY);
            DrawText("Click me!", button.x + 10, button.y + 10, 20, BLACK);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button))
        {
            PlaySound(sound);
        }

        EndDrawing();
    }

    UnloadSound(sound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
