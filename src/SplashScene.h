#ifndef SPLASHSCENE_H
#define SPLASHSCENE_H

#include "GameScene.h"

class SplashScene : public GameScene {
public:
    SplashScene();
    virtual void Load() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;
    virtual void Unload() override;
};

#endif // SPLASHSCENE_H
