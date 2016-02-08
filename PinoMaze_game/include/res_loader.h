#ifndef __RES_LOADER_H__
#define __RES_LOADER_H__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <string>

bool openResourceFile(const char *filename);
void closeResourceFile();

std::string loadStringFromFile(const char *filename);

std::string loadStringFromResource(const char *RES_ID);
SDL_Surface *loadImageFromResource(const char *RES_ID);
Mix_Chunk *loadWaveFromResource(const char *RES_ID);
Mix_Music *loadMusicFromResource(const char *RES_ID);

#endif
