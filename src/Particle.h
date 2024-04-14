#include "raylib.h"
#include <cmath>
#include <iostream>
class Particle {
public:
    Vector2 position = { 0, 0 };
    Vector2 velocity = { 0, 0 };
    float radius = 0;
    float life = 0;     
    Color color = WHITE;   

    inline Particle(Vector2 pos, Vector2 vel, float rad, Color col, float lifeTime) :
        position(pos), velocity(vel), radius(rad), life(lifeTime), color(col) {}

    inline void Update(float deltaTime) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime + 0.1f * sin(GetTime()) * 5.0f;
        life -= deltaTime;
        radius -= deltaTime * 5.0f;
        if (radius < 0) radius = 0;
    }

    inline void Draw() const {
        DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), radius, color);
    }

    inline bool IsAlive() const {
        return life > 0;
    }
};
