#ifndef __RES_LOADER_H__
#define __RES_LOADER_H__

#include "../resource.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <string>

std::string loadStringFromFile(const char *filename);
std::string loadStringFromResource(int RES_ID);

SDL_Surface *loadImageFromResources(int RES_ID);
Mix_Chunk *loadWaveFromResource(int RES_ID);
Mix_Music *loadMusicFromResource(int RES_ID);

#endif
