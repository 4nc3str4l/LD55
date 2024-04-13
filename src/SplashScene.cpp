#include "SplashScene.h"
#include <iostream>
#include "utils.h"
#include "constants.h"
#include "SceneManager.h"
#include "Scheduler.h"

SplashScene::SplashScene() : GameScene("Splash Scene") {}

void SplashScene::Load() {
    std::cout << "Loading Splash Scene resources..." << std::endl;
    background =  LoadTexture("resources/splash.png");

    titleSize = MeasureText("Spring MUST Come", 40);
    pressEnterToStartSize = MeasureText("Press [SPACE] to start", 20);
}

void SplashScene::Update(float deltaTime)
{
    if(IsKeyPressed(KEY_SPACE))
    {
        if(!changeSceneSheduled)
        {
            changeSceneSheduled = true;
            Scheduler::SetTimeout([](){
                SceneManager::GetInstance().ChangeScene("InGame");
            }, 2.0f);
        }
    }

    if (changeSceneSheduled) {
        fadeOutOpacity += deltaTime;
        if (fadeOutOpacity > 1.0f) fadeOutOpacity = 1.0f;
    }
}

void SplashScene::Render()
{
    DrawTexture(background, 0, 0, WHITE);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    DrawRichText("Spring <color=0,255,155,255>MUST</color> Come", SCREEN_WIDTH / 2 - titleSize / 2, 100, 40, WHITE);
    DrawRichText("Press <color=0,255,155,255> [SPACE] </color> to start", SCREEN_WIDTH / 2 - pressEnterToStartSize /2, SCREEN_HEIGHT / 2 + 100, 20, WHITE);

    DrawRichText("Made with <color=255,0,0,255>Love</color> in 48h by <color=0,255,155,255>Cristian Muriel</color> for Ludum Dare 55", 10, SCREEN_HEIGHT - 30, 20, WHITE);

    if (changeSceneSheduled) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, fadeOutOpacity));
    }
}

void SplashScene::Unload() {
    std::cout << "Unloading Splash Scene resources..." << std::endl;
    UnloadTexture(background);
}
