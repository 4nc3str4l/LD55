#pragma once

#include <string>

class GameScene {
protected:
public:
    GameScene(const std::string& name) : name(name) {}
    virtual ~GameScene() {}
    virtual void Load() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Unload() = 0;
public:
    std::string name;
};
