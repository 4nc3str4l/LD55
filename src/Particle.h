#include "raylib.h"
#include <cmath>
#include <iostream>
class Particle {
public:
    Vector2 position;
    Vector2 velocity;
    float radius;
    float life;
    Color color;

    inline Particle(Vector2 pos, Vector2 vel, float rad, Color col, float lifeTime) :
        position(pos), velocity(vel), radius(rad), color(col), life(lifeTime) {}

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
