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

    backgroundMusic = LoadMusicStream("resources/bg_music.mp3");;

    if(backgroundMusic.stream.buffer == nullptr)
    {
        std::cout << "Error loading music" << std::endl;
    }
    PlayMusicStream(backgroundMusic);

    distortionShader = LoadDistorionShader();
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(distortionShader, GetShaderLocation(distortionShader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

    timeElapsed = 0.0f;
    fadeOutOpacity = 0.0f;
    changeSceneSheduled = false;
    fadeTime = 0.5f;
}

void SplashScene::Update(float deltaTime)
{
    timeElapsed += deltaTime;
    SetShaderValue(distortionShader, GetShaderLocation(distortionShader, "time"), &timeElapsed, SHADER_UNIFORM_FLOAT);
    

    UpdateMusicStream(backgroundMusic);
    if(IsKeyPressed(KEY_SPACE))
    {
        if(!changeSceneSheduled)
        {
            changeSceneSheduled = true;
            Scheduler::SetTimeout([](){
                SceneManager::GetInstance().ChangeScene("InGame");
            }, fadeTime);
        }
    }

    if (changeSceneSheduled) {
        fadeOutOpacity += deltaTime * 1 / fadeTime;
        if (fadeOutOpacity > 1.0f) fadeOutOpacity = 1.0f;
    }


}

void SplashScene::Render()
{
    BeginShaderMode(distortionShader);
    DrawTexture(background, 0, 0, WHITE);
    EndShaderMode();

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));

    DrawRichText("Spring <color=0,255,155,255>MUST</color> Come", SCREEN_WIDTH / 2 - titleSize / 2, 100, 40, WHITE);
    DrawRichText("Press <color=0,255,155,255> [SPACE] </color> to start", SCREEN_WIDTH / 2 - pressEnterToStartSize /2, SCREEN_HEIGHT / 2 + 100, 20, WHITE);

    DrawRichText("Made with <color=255,0,0,255>Love</color> in 48h by <color=0,255,155,255>Cristian Muriel</color> for Ludum Dare 55", 10, SCREEN_HEIGHT - 30, 20, WHITE);

    if (changeSceneSheduled) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, fadeOutOpacity));
    }

    EnableVolumeOptions(true);
}

void SplashScene::Unload() {
    std::cout << "Unloading Splash Scene resources..." << std::endl;
    UnloadTexture(background);
    UnloadMusicStream(backgroundMusic);
    UnloadShader(distortionShader);
}
