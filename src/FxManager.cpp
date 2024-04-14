#include "FxManager.h"
#include <iostream>

void FXManager::Init()
{
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
