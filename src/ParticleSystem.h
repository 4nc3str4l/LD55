#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Particle.h"

#include <vector>

class ParticleSystem {
private:
    std::vector<Particle> particles;

public:
    void Emit(Vector2 position, Vector2 velocity, float radius, Color color, float lifeTime) {
        particles.emplace_back(position, velocity, radius, color, lifeTime);
    }

    void Update(float deltaTime);
    void Draw();
};



#endif //PARTICLESYSTEM_H
