#include "SoundManager.h"
#include <iostream>

void SoundManager::Init()
{
    // Load all sounds
    sounds[SFX_DRY] = LoadSound("resources/dry.wav");
    sounds[SFX_FREEZE] = LoadSound("resources/freeze.wav");
    sounds[SFX_GRAB] = LoadSound("resources/grab.wav");
    sounds[SFX_GRASS] = LoadSound("resources/grass.wav");
    sounds[SFX_HIT] = LoadSound("resources/hit.wav");
    sounds[SFX_RELEASE] = LoadSound("resources/release.wav");
    sounds[SFX_HEAL] = LoadSound("resources/heal.wav");
    sounds[SFX_VICTORY] = LoadSound("resources/victory.wav");
}

void SoundManager::Update(float deltaTime)
{

    for (auto &soundTimer : soundTimers)
    {
        if (soundTimer.second > 0)
        {
            soundTimer.second -= deltaTime;
        }
    }
}

void SoundManager::Cleanup()
{

    for (auto &sound : sounds)
    {
        UnloadSound(sound.second);
    }
    sounds.clear();
}

void SoundManager::PlaySound(const std::string &soundFile, float volume, float pitchVariance)
{
    if (soundTimers[soundFile] > 0)
    {
        return;
    }
    soundTimers[soundFile] = 0.05;

    if (sounds.find(soundFile) == sounds.end())
    {
        sounds[soundFile] = LoadSound(soundFile.c_str());
    }
    float pitch = 1.0f + (GetRandomValue(-100, 100) / 1000.0f) * pitchVariance;
    SetSoundPitch(sounds[soundFile], pitch);
    SetSoundVolume(sounds[soundFile], volume);
    ::PlaySound(sounds[soundFile]);
}
