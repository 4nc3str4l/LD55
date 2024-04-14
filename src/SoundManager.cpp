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

    gameMusic = LoadMusicStream("resources/in_game_music.mp3");
    titleMusic = LoadMusicStream("resources/bg_music.mp3");
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

    if(currentMusicStream != nullptr)
    {
        UpdateMusicStream(*currentMusicStream);
    }
}

void SoundManager::Cleanup()
{
    StopMusic();

    for (auto &sound : sounds)
    {
        UnloadSound(sound.second);
    }
    sounds.clear();

    UnloadMusicStream(gameMusic);
    UnloadMusicStream(titleMusic);
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

void SoundManager::PlayMusic(Music &music, float volume)
{
    if (currentMusicStream != nullptr) {
        StopMusicStream(*currentMusicStream); // Stop current music if it is playing
    }
    
    currentMusicStream = &music; // Set the new music stream
    PlayMusicStream(music); // Play the new music
    // Set the volume of the music
    SetMusicVolume(music, volume);
}

void SoundManager::StopMusic()
{
    if (currentMusicStream != nullptr) {
        StopMusicStream(*currentMusicStream); // Stop the music
        currentMusicStream = nullptr;
    }
}