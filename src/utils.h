#pragma once

#include <random>
#include <iostream>
#include "raylib.h"
#include "constants.h"

inline std::random_device rd;
inline std::mt19937 mt(rd());

inline float GetRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(mt);
}

inline int GetRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

inline Vector2 GetRandomVector(float minX, float minY, float maxX, float maxY) {
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    return Vector2{distX(mt), distY(mt)};
}

// Helper function to convert color string "r,g,b,a" to Color structure
inline Color ParseColor(const std::string& colorString) {
    int r, g, b, a;
    sscanf(colorString.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
    return { static_cast<unsigned char>(r),
             static_cast<unsigned char>(g),
             static_cast<unsigned char>(b),
             static_cast<unsigned char>(a) };
}

inline std::string FormatText(const char* format, int value) {
    char buffer[128]; // Ensure buffer is large enough for your formatting needs
    snprintf(buffer, sizeof(buffer), format, value);
    return std::string(buffer);
}


inline void DrawRichText(const char* text, int posX, int posY, int fontSize, Color defaultColor) {
    std::string strText = text;
    size_t start = 0;
    size_t end = 0;
    Color currentColor = defaultColor;
    int currentX = posX;

    while (start < strText.length()) {
        end = strText.find("<color=", start);
        size_t endColorTag = strText.find("</color>", start);

        if (end != std::string::npos && (end < endColorTag || endColorTag == std::string::npos)) {
            // Draw the text before the <color=...> tag
            DrawText(strText.substr(start, end - start).c_str(), currentX, posY, fontSize, currentColor);
            currentX += MeasureText(strText.substr(start, end - start).c_str(), fontSize);

            // Find the closing bracket of the color tag
            size_t closeBracket = strText.find('>', end);
            if (closeBracket != std::string::npos) {
                std::string color = strText.substr(end + 7, closeBracket - (end + 7));
                currentColor = ParseColor(color);
                start = closeBracket + 1;
            } else {
                // Malformed tag
                break;
            }
        } else if (endColorTag != std::string::npos) {
            // Draw the text before the </color> tag
            DrawText(strText.substr(start, endColorTag - start).c_str(), currentX, posY, fontSize, currentColor);
            currentX += MeasureText(strText.substr(start, endColorTag - start).c_str(), fontSize);

            currentColor = defaultColor;
            start = endColorTag + 8;
        } else {
            // No more tags, draw the rest of the text
            DrawText(strText.substr(start).c_str(), currentX, posY, fontSize, currentColor);
            break;
        }
    }
}

inline Shader LoadDistorionShader() {
    Shader distortionShader;
    // if platform is web
#if defined(PLATFORM_WEB)
    distortionShader = LoadShader(NULL, "resources/distortion_web.fs");
#else
    distortionShader = LoadShader(NULL, "resources/distortion.fs");
#endif
    return distortionShader;
}

inline Shader LoadEntitiesShader() {
    Shader entitiesShader;
    // if platform is web
#if defined(PLATFORM_WEB)
    entitiesShader = LoadShader(NULL, "resources/entities_web.fs");
#else
    entitiesShader = LoadShader(NULL, "resources/entities.fs");
#endif
    return entitiesShader;
}


inline void EnableVolumeOptions(bool render)
{
    if(render)
    {
        DrawRichText("Press <color=0,255,155,255> [V] </color> to mute/unmute the audio", SCREEN_WIDTH - 200, SCREEN_HEIGHT - 30, 10, WHITE);
        DrawRichText("Use <color=0,255,155,255> [U] </color> or <color=0,255,155,255> [J] </color> to increase or decrease the volume", SCREEN_WIDTH - 280, SCREEN_HEIGHT - 60, 10, WHITE);
    }

    if (IsKeyReleased(KEY_V))
    {
        SetMasterVolume(GetMasterVolume() > 0.0f ? 0.0f : 1.0f);
    }

    if (IsKeyReleased(KEY_U))
    {
        SetMasterVolume(GetMasterVolume() + 0.1f);
    }

    if (IsKeyReleased(KEY_J))
    {
        SetMasterVolume(GetMasterVolume() - 0.1f);
    }
}
