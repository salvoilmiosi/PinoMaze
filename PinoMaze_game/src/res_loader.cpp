#include "res_loader.h"

#include <Windows.h>
#include <fstream>

using namespace std;

std::string loadStringFromFile(const char *filename) {
	ifstream ifs(filename);
	string str;

	ifs.seekg(0, ios::end);
	str.reserve((size_t)ifs.tellg());
	ifs.seekg(0, ios::beg);

	str.assign((istreambuf_iterator<char>(ifs)),
		istreambuf_iterator<char>());

	return str;
}

static SDL_RWops *loadResourceRW(int RES_ID, const char *RES_TYPE) {
	HMODULE hModule = GetModuleHandle(nullptr);

	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(RES_ID), RES_TYPE);
	if (!hRes) {
		fprintf(stderr, "Could not load resource %d %s\n", RES_ID, RES_TYPE);
		return nullptr;
	}
	int res_size = SizeofResource(hModule, hRes);

	HGLOBAL hgRes = LoadResource(hModule, hRes);
	char *res_data = (char*) LockResource(hgRes);

	return SDL_RWFromConstMem(res_data, res_size);
}

std::string loadStringFromResource(int RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "TEXTFILE");

	if (!data)
		return "";

	size_t datasize = (size_t)SDL_RWsize(data);
	if (datasize <= 0)
		return "";

	char *buf = new char[datasize + 1];
	memset(buf, 0, datasize + 1);

	SDL_RWread(data, buf, datasize, 1);
	SDL_RWclose(data);

	std::string str(buf);
	delete[]buf;

	return str;
}

SDL_Surface *loadImageFromResources(int RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "PNG");

	if (data) {
		SDL_Surface *surf = IMG_LoadTyped_RW(data, 1, "PNG");
		if (!surf) {
			fprintf(stderr, "Could not load image %d: %s\n", RES_ID, IMG_GetError());
		}
		return surf;
	}
	return nullptr;
}

Mix_Chunk *loadWaveFromResource(int RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "WAVE");

	if (data) {
		Mix_Chunk *chunk = Mix_LoadWAV_RW(data, 1);
		if (!chunk) {
			fprintf(stderr, "Could not load audio %d: %s\n", RES_ID, Mix_GetError());
		}
		return chunk;
	}
	return nullptr;
}

Mix_Music *loadMusicFromResource(int RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "OGG");

	if (data) {
		Mix_Music *mus = Mix_LoadMUSType_RW(data, MUS_OGG, 1);
		if (!mus) {
			fprintf(stderr, "Could not load music %d: %s\n", RES_ID, Mix_GetError());
		}
		return mus;
	}
	return nullptr;
}