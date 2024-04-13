#pragma once

#include <random>
#include <iostream>
#include "raylib.h"

std::random_device rd;
std::mt19937 mt(rd());

float GetRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(mt);
}

int GetRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

Vector2 GetRandomVector(float minX, float minY, float maxX, float maxY) {
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    return Vector2{distX(mt), distY(mt)};
}
