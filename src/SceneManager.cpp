#include "SceneManager.h"
#include <iostream>

void SceneManager::AddScene(const std::string& name, std::shared_ptr<GameScene> scene) {
    scenes[name] = scene;
}

void SceneManager::ChangeScene(const std::string& name) {
    auto it = scenes.find(name);
    if (it != scenes.end()) {
        if (currentScene != nullptr) {
            currentScene->Unload();
        }
        currentScene = it->second;
        currentScene->Load();
    } else {
        std::cerr << "Scene '" << name << "' not found." << std::endl;
    }
}

void SceneManager::RemoveScene(const std::string& name) {
    if (currentScene && currentScene->name == name) {
        currentScene->Unload();
        currentScene = nullptr;
    }
    scenes.erase(name);
}

void SceneManager::LoadCurrentScene() {
    if (currentScene) {
        currentScene->Load();
    }
}

void SceneManager::UpdateCurrentScene(float deltaTime) {
    if (currentScene) {
        currentScene->Update(deltaTime);
    }
}

void SceneManager::RenderCurrentScene() {
    if (currentScene) {
        currentScene->Render();
    }
}

void SceneManager::UnloadCurrentScene() {
    if (currentScene) {
        currentScene->Unload();
    }
}
