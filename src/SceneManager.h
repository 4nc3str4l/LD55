#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <map>
#include <memory>
#include <string>
#include "GameScene.h"

class SceneManager {
private:
    std::map<std::string, std::shared_ptr<GameScene>> scenes;
    std::shared_ptr<GameScene> currentScene;

    SceneManager() {}

public:
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    static SceneManager& GetInstance() {
        static SceneManager instance;
        return instance;
    }

    void AddScene(const std::string& name, std::shared_ptr<GameScene> scene);
    void ChangeScene(const std::string& name);
    void RemoveScene(const std::string& name);

    void LoadCurrentScene();
    void UpdateCurrentScene(float deltaTime);
    void RenderCurrentScene();
    void UnloadCurrentScene();
};

#endif // SCENEMANAGER_H
