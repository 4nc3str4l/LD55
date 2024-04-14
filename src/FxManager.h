#ifndef FXMANAGER_H

#define FXMANAGER_H

#include "raylib.h"
#include <vector>
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

public:
    static void Init();
    static void Update(float deltaTime);
    static void Draw();
    static void DrawEffectsInWorld();
    static void AddFadeRect(Rectangle rect, Color color, float duration, bool);
    static void Cleanup();
};

#endif // FXMANAGER_H