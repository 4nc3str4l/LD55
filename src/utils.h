#pragma once

#include <random>
#include <iostream>
#include "raylib.h"

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