#ifndef INGAMESCENE_H
#define INGAMESCENE_H

#include "GameScene.h"
#include "raylib.h"
#include <vector>   
#include "world.h"

enum class GameState 
{
    STARTING,
    PLAYING,
    IN_MENU,
    VICTORY,
    GAME_OVER
};

class InGameScene : public GameScene {
public:
    InGameScene();
    virtual void Load() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;
    virtual void Unload() override;
private:
    void DrawStartingUI();
    void DrawInGameUI(const World * world);
    void DrawPlaying(World * world);
    void DrawGameOverUI();
    void DrawInMenuUI(World * world);
    void DrawVictoryUI();

    void UpdateStarting(float deltaTime);
    void UpdatePlaying(float deltaTime);
    void UpdateGameOver(float deltaTime);
    void UpdateVictory(float deltaTime);
    void UpdateInMenuUI(float deltaTime);

    void RegisterWorld(int level);
    World* GetWorld(int level);

private:
    World* world;
    Shader distortionShader;
    Shader entitiesShader;
    float timeElapsed = 0.0f;
    Music music;
    GameState gameState = GameState::STARTING;
    Texture2D background;

    std::vector<RegisteredWorld> registeredWorlds;
    size_t currentLevel = 2;
};

#endif // INGAMESCENE_H
