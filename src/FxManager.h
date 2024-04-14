#ifndef FXMANAGER_H

#define FXMANAGER_H

#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <string>

struct FadeRectangle
{
    Rectangle rect;
    Color color;
    float endTime;
    float fadeDuration;
};

class FXManager
{
private:
    inline static std::vector<FadeRectangle> fadeRects;
    inline static std::vector<FadeRectangle> fadeEffectsInWorld;
    inline static std::unordered_map<std::string, Sound> sounds;
    inline static std::unordered_map<std::string, float> soundTimers;

public:
    static void Init();
    static void Update(float deltaTime);
    static void Draw();
    static void DrawEffectsInWorld();
    static void AddFadeRect(Rectangle rect, Color color, float duration, bool);
    static void PlaySound(std::string soundFile, float volume, float pitchVariance);
    static void Cleanup();
};

#endif // FXMANAGER_H