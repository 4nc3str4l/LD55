#ifndef SPLASHSCENE_H
#define SPLASHSCENE_H

#include "GameScene.h"

#include "raylib.h"

class SplashScene : public GameScene {
public:
    SplashScene();
    virtual void Load() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;
    virtual void Unload() override;
private:
    Texture2D background;

    int titleSize;
    int pressEnterToStartSize;
    bool changeSceneSheduled = false;
    float fadeOutOpacity = 0.0f;
};

#endif // SPLASHSCENE_H
