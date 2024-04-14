#ifndef SOUND_MANAGER_H

#define SOUND_MANAGER_H

#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <string>

inline const std::string SFX_DRY = "dry";
inline const std::string SFX_FREEZE = "freeze";
inline const std::string SFX_GRAB = "grab";
inline const std::string SFX_GRASS = "grass";
inline const std::string SFX_HIT = "hit";
inline const std::string SFX_RELEASE = "release";
inline const std::string SFX_HEAL = "heal";
inline const std::string SFX_VICTORY = "victory";

class SoundManager
{
public:
    inline static Music titleMusic;
    inline static Music gameMusic;
private:
    inline static std::unordered_map<std::string, Sound> sounds;
    inline static std::unordered_map<std::string, float> soundTimers;
    inline static Music *currentMusicStream = nullptr;
public:
    static void Init();
    static void Update(float deltaTime);
    static void PlaySound(const std::string &soundFile, float volume, float pitchVariance);
    static void Cleanup();
    static void PlayMusic(Music &music, float volume);
     static void StopMusic();
};

#endif // SOUND_MANAGER_H