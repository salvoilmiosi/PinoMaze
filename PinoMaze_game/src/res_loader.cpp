#include "res_loader.h"

#include <SDL2/SDL_image.h>

#include <string>
#include <fstream>

using namespace std;

#include <Windows.h>

const char *loadStringFromFile(const char *filename) {
	ifstream ifs(filename);
	string str;

	ifs.seekg(0, ios::end);
	str.reserve((int) ifs.tellg());
	ifs.seekg(0, ios::beg);

	str.assign((istreambuf_iterator<char>(ifs)),
		istreambuf_iterator<char>());

	return str.c_str();
}

const char *loadStringFromResource(int RES_ID) {
    HMODULE hModule = GetModuleHandle(nullptr);

    HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(RES_ID), "TEXTFILE");
    if (!hRes) {
        return nullptr;
    }
    unsigned int res_size = SizeofResource(hModule, hRes);
    char *ret = new char[res_size+1];

    HGLOBAL hgRes = LoadResource(hModule, hRes);
    char *res_data = (char *)LockResource(hgRes);
    memcpy(ret, res_data, res_size+1);

    ret[res_size] = '\0';
    return ret;
}

SDL_Surface *loadImageFromResources(int RES_ID) {
	HMODULE hModule = GetModuleHandle(nullptr);

	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(RES_ID), "PNG");
	if (!hRes) {
        return nullptr;
	}
	unsigned int res_size = SizeofResource(hModule, hRes);

	HGLOBAL hgRes = LoadResource(hModule, hRes);
	unsigned char* res_data = (unsigned char*)LockResource(hgRes);

	return IMG_LoadTyped_RW(SDL_RWFromConstMem(res_data, res_size), 1, "PNG");
}