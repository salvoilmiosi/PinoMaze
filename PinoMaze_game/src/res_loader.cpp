#include "res_loader.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <fstream>

using namespace std;

#include <Windows.h>

std::string loadStringFromFile(const char *filename) {
	ifstream ifs(filename);
	string str;

	ifs.seekg(0, ios::end);
	str.reserve((int)ifs.tellg());
	ifs.seekg(0, ios::beg);

	str.assign((istreambuf_iterator<char>(ifs)),
		istreambuf_iterator<char>());

	return str;
}

static SDL_RWops *loadResourceRW(int RES_ID, const char *RES_TYPE) {
	HMODULE hModule = GetModuleHandle(nullptr);

	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(RES_ID), RES_TYPE);
	if (!hRes) {
		char errstr[128];
		sprintf_s(errstr, "Could not load resource %d %s", RES_ID, RES_TYPE);
		MessageBox(nullptr, errstr, "Error", MB_ICONERROR);
		return nullptr;
	}
	unsigned int res_size = SizeofResource(hModule, hRes);

	HGLOBAL hgRes = LoadResource(hModule, hRes);
	unsigned char* res_data = (unsigned char*)LockResource(hgRes);

	return SDL_RWFromConstMem(res_data, res_size);
}

std::string loadStringFromResource(int RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "TEXTFILE");

	if (!data)
		return "";

	size_t datasize = (size_t)SDL_RWsize(data);
	if (datasize < 0)
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

	if (data)
		return IMG_LoadTyped_RW(data, 1, "PNG");
	return nullptr;
}

Mix_Chunk *loadWaveFromResource(int RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "WAVE");

	if (data) 
		return Mix_LoadWAV_RW(data, 1);
	return nullptr;
}