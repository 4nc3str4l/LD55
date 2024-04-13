#include "raylib.h"
#include "SplashScene.h"
#include "InGameScene.h"
#include "SceneManager.h"
#include "constants.h"
#include "Scheduler.h"

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    InitAudioDevice();

    // Create the scenes and add them to the scene manager
    SceneManager& sceneManager = SceneManager::GetInstance();
    sceneManager.AddScene("Splash", std::make_shared<SplashScene>());
    sceneManager.AddScene("InGame", std::make_shared<InGameScene>());
    sceneManager.ChangeScene("Splash");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        Scheduler::Update(deltaTime);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        sceneManager.UpdateCurrentScene(deltaTime);
        sceneManager.RenderCurrentScene();

        EndDrawing();
    }

    // Clear scheduler for security
    Scheduler::Clear();

    sceneManager.UnloadCurrentScene();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

