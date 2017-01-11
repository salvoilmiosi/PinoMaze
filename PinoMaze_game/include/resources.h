#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include "../../resource_pack/include/resource_load.h"

#include <SDL2/SDL_image.h>

#include <string>

using namespace std;

#include "sound.h"

bool openResourceFile(const char *dir, const char *filename);

template<typename ... T>
inline bool openResourceFile(const char *dir, const char *first, T ... filenames) {
	return openResourceFile(dir, first) ? openResourceFile(dir, filenames ...) : false;
}

std::string loadStringFromResource(const char *RES_ID);
SDL_Surface *loadImageFromResource(const char *RES_ID);
bool loadWaveFromResource(sound &snd, const char *RES_ID);
bool loadMusicFromResource(music &mus, const char *RES_ID);

#endif // __RESOURCES_H__
