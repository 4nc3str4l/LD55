#ifndef INGAMESCENE_H
#define INGAMESCENE_H

#include "GameScene.h"
#include "raylib.h"
struct World;

class InGameScene : public GameScene {
public:
    InGameScene();
    virtual void Load() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;
    virtual void Unload() override;
private:
    World* world;
    Shader distortionShader;
    Shader entitiesShader;
    float timeElapsed = 0.0f;
};

#endif // INGAMESCENE_H
