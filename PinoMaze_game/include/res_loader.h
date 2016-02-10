#ifndef __RES_LOADER_H__
#define __RES_LOADER_H__

#include <SDL2/SDL_image.h>

#include <string>

using namespace std;

#include "sound.h"
#include "buffer.h"

bool openResourceFile(const char *filename);

template<typename ... T>
inline bool openResourceFile(const char *first, T ... filenames) {
	return openResourceFile(first) ? openResourceFile(filenames ...) : false;
}

std::string loadStringFromFile(const char *filename);

std::string loadStringFromResource(const char *RES_ID);
SDL_Surface *loadImageFromResource(const char *RES_ID);
bool loadWaveFromResource(sound &snd, const char *RES_ID);
bool loadMusicFromResource(music &mus, const char *RES_ID);

#endif
