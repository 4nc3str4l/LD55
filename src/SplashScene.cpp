#include "SplashScene.h"
#include <iostream>

SplashScene::SplashScene() : GameScene("Splash Scene") {}

void SplashScene::Load() {
    std::cout << "Loading Splash Scene resources..." << std::endl;
}

void SplashScene::Update(float deltaTime) {
    std::cout << "Updating Splash Scene: " << deltaTime << " seconds" << std::endl;
}

void SplashScene::Render() {
    std::cout << "Rendering Splash Scene..." << std::endl;
}

void SplashScene::Unload() {
    std::cout << "Unloading Splash Scene resources..." << std::endl;
}
