#pragma once
#include "SFML/Audio.hpp"

void InitSoundVec(int limit);
void CleanSoundVec();
void PlaySoundVec(sf::SoundBuffer *bf, float volume = 100.f, float pitch = 1.f);
void UpdateSoundVec();
int NbSoundVec();