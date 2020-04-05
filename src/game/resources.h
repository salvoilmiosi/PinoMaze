#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <SDL2/SDL_image.h>
#include <string>

#include "resource_load.h"

#include "engine/sound.h"

bool openResourceFile(const char *dir, const char *filename);

template<typename ... T>
inline int openResourceFiles(const char *dir, T ... filenames) {
	return (openResourceFile(dir, filenames) + ...);
}

SDL_Surface *loadImageFromResource(const char *RES_ID);
bool loadWaveFromResource(sound &snd, const char *RES_ID);
bool loadMusicFromResource(music &mus, const char *RES_ID);

#endif // __RESOURCES_H__
