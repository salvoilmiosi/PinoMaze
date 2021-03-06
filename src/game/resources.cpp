#include "resources.h"

#include <vector>

bool openResourceFile(const char *dir, const char *filename) {
    std::string path;
    if (dir) {
        path = dir;
        path = path.substr(0, 1 + path.find_last_of("\\/"));
    }
    path += filename;
    return openResourceFile(path.c_str());
}

SDL_Surface *loadImageFromResource(const char *RES_ID) {
	SDL_RWops *data = getResourceRW(RES_ID);

	if (data) {
		SDL_Surface *surf = IMG_LoadTyped_RW(data, 1, "PNG");
		if (!surf) {
			fprintf(stderr, "Could not load image %s: %s\n", RES_ID, IMG_GetError());
		}
		return surf;
	}

	return nullptr;
}

bool loadWaveFromResource(sound &snd, const char *RES_ID) {
	if (!snd.loadChunk(getResourceRW(RES_ID))) {
		fprintf(stderr, "Could not load audio %s: %s\n", RES_ID, Mix_GetError());
		return false;
	}
	return true;
}

bool loadMusicFromResource(music &mus, const char *RES_ID) {
	if (!mus.loadMusic(getResourceRW(RES_ID))) {
		fprintf(stderr, "Could not load music %s: %s\n", RES_ID, Mix_GetError());
		return false;
	}
	return true;
}