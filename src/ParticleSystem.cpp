//
// Created by ancestral on 13.04.24.
//

#include "ParticleSystem.h"

void ParticleSystem::Update(float deltaTime) {

    for (auto it = particles.begin(); it != particles.end(); ) {
        it->Update(deltaTime);
        if (!it->IsAlive()) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }

}

void ParticleSystem::Draw() {
    for (auto& particle : particles) {
        particle.Draw();
    }
}