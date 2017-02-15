#include "resources.h"

#include <vector>

bool openResourceFile(const char *dir, const char *filename) {
    std::string path;
    if (dir) {
        path = dir;
        path = path.substr(0, 1 + path.find_last_of("\\/"));
    }
    path += filename;
    return openResourceFile(path);
}

string loadStringFromResource(const char * RES_ID) {
	SDL_RWops *rw = getResourceRW(RES_ID);
	if (!rw) {
		return "";
	}

	Sint64 res_size = SDL_RWsize(rw);
	char *res = (char*)malloc(res_size + 1);
	memset(res, 0, res_size + 1);

	Sint64 nb_read_total = 0, nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		free(res);
		return "";
	}

	res[nb_read_total] = '\0';
	return std::string(res, res_size);
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