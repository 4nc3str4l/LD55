#include "FxManager.h"
#include <iostream>

void FXManager::Init()
{
    // Load all sounds
}

void FXManager::Update(float deltaTime)
{
    for (auto it = fadeRects.begin(); it != fadeRects.end();)
    {
        it->endTime -= deltaTime;
        if (it->endTime <= 0)
        {
            it = fadeRects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (auto it = fadeEffectsInWorld.begin(); it != fadeEffectsInWorld.end();)
    {
        it->endTime -= deltaTime;
        if (it->endTime <= 0)
        {
            it = fadeEffectsInWorld.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (auto &soundTimer : soundTimers)
    {
        if (soundTimer.second > 0)
        {
            soundTimer.second -= deltaTime;
        }
    }
}

void FXManager::Draw()
{
    for (const auto &rect : fadeRects)
    {
        DrawRectangleRec(rect.rect, Fade(rect.color, rect.endTime / rect.fadeDuration));
    }
}

void FXManager::DrawEffectsInWorld()
{
    for (const auto &rect : fadeEffectsInWorld)
    {
        DrawRectangleRec(rect.rect, Fade(rect.color, rect.endTime / rect.fadeDuration));
    }
}

void FXManager::Cleanup()
{

    for (auto &sound : sounds)
    {
        UnloadSound(sound.second);
    }
    sounds.clear();
    fadeRects.clear();
    fadeEffectsInWorld.clear();
}

void FXManager::AddFadeRect(Rectangle rect, Color color, float duration, bool inWorld)
{
    if (inWorld)
    {
        fadeEffectsInWorld.push_back({rect, color, duration, duration});
    }
    else
    {
        fadeRects.push_back({rect, color, duration, duration});
    }
}

void FXManager::PlaySound(std::string soundFile, float volume, float pitchVariance)
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
