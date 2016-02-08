#include "res_loader.h"

#include <fstream>
#include <vector>

using namespace std;

static const int ID_MAXSIZE = 32;

struct resource {
	size_t size;
	size_t ptr;
	char res_id[ID_MAXSIZE];
};

static vector<resource> resFiles;
static ifstream resourceIfs;

static unsigned int readInt(ifstream &ifs) {
	char data[4];
	ifs.read(data, 4);
	unsigned int num =
		(data[0] << 24 & 0xff000000) |
		(data[1] << 16 & 0x00ff0000) |
		(data[2] << 8 & 0x0000ff00) |
		(data[3] & 0x000000ff);
	return num;
}

bool openResourceFile(const char *filename) {
	resourceIfs.open(filename, ios::binary);

	if (resourceIfs.fail()) {
		return false;
	}

	if (readInt(resourceIfs) != 0x255435f4) {
		return false;
	}

	resFiles.resize(readInt(resourceIfs));

	for (resource &res : resFiles) {
		resourceIfs.read(res.res_id, ID_MAXSIZE);
		res.size = readInt(resourceIfs);
		res.ptr = readInt(resourceIfs);
	}

	return true;
}

void closeResourceFile() {
	resourceIfs.close();
}

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

static SDL_RWops *loadResourceRW(const char *RES_ID, const char *RES_TYPE) {
	for (resource &res : resFiles) {
		if (strcmp(res.res_id, RES_ID) == 0) {
			char *data = new char[res.size];

			resourceIfs.seekg(res.ptr);
			resourceIfs.read(data, res.size);

			return SDL_RWFromConstMem(data, res.size);
		}
	}
	return nullptr;
}

std::string loadStringFromResource(const char * RES_ID) {
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

SDL_Surface *loadImageFromResource(const char *RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "PNG");

	if (data) {
		SDL_Surface *surf = IMG_LoadTyped_RW(data, 1, "PNG");
		if (!surf) {
			fprintf(stderr, "Could not load image %s: %s\n", RES_ID, IMG_GetError());
		}
		return surf;
	}
	return nullptr;
}

Mix_Chunk *loadWaveFromResource(const char *RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "WAVE");

	if (data) {
		Mix_Chunk *chunk = Mix_LoadWAV_RW(data, 1);
		if (!chunk) {
			fprintf(stderr, "Could not load audio %s: %s\n", RES_ID, Mix_GetError());
		}
		return chunk;
	}
	return nullptr;
}

Mix_Music *loadMusicFromResource(const char *RES_ID) {
	SDL_RWops *data = loadResourceRW(RES_ID, "OGG");

	if (data) {
		Mix_Music *mus = Mix_LoadMUSType_RW(data, MUS_OGG, 1);
		if (!mus) {
			fprintf(stderr, "Could not load music %s: %s\n", RES_ID, Mix_GetError());
		}
		return mus;
	}
	return nullptr;
}