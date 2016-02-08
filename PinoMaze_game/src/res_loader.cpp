#include "res_loader.h"

#include <fstream>
#include <map>
#include <memory>

using namespace std;

static const int ID_MAXSIZE = 32;

struct resource {
	size_t size = 0;
	size_t ptr = 0;
	const char *filename;
};

map<string, resource> resFiles;

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
	ifstream ifs(filename, ios::binary);

	if (ifs.fail()) {
		return false;
	}

	if (readInt(ifs) != 0x255435f4) {
		return false;
	}

	int numRes = readInt(ifs);
	char res_id[ID_MAXSIZE];

	resource res;
	while (numRes > 0) {
		memset(&res, 0, sizeof(res));
		memset(res_id, 0, sizeof(res_id));

		ifs.read(res_id, ID_MAXSIZE);

		res.size = readInt(ifs);
		res.ptr = readInt(ifs);
		res.filename = filename;

		resFiles[res_id] = res;

		--numRes;
	}

	return true;
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

unique_ptr<buffer> loadResourceBuffer(const char *RES_ID) {
	auto it = resFiles.find(RES_ID);

	if (it != resFiles.end()) {
		resource &res = it->second;
		char *data = new char[res.size];

		ifstream ifs(res.filename, ios::binary);
		ifs.seekg(res.ptr);
		ifs.read(data, res.size);

		return make_unique<buffer>(data, res.size);
	}

	return nullptr;
}

string loadStringFromResource(const char * RES_ID) {
	unique_ptr<buffer> data = loadResourceBuffer(RES_ID);

	if (!data)
		return "";

	return string(data->data, data->size);
}

SDL_Surface *loadImageFromResource(const char *RES_ID) {
	unique_ptr<buffer> data = loadResourceBuffer(RES_ID);

	if (data) {
		SDL_Surface *surf = IMG_LoadTyped_RW(data->rw, 0, "PNG");
		if (!surf) {
			fprintf(stderr, "Could not load image %s: %s\n", RES_ID, IMG_GetError());
		}
		return surf;
	}
	return nullptr;
}

bool loadWaveFromResource(sound &snd, const char *RES_ID) {
	if (!snd.loadChunk(loadResourceBuffer(RES_ID))) {
		fprintf(stderr, "Could not load audio %s: %s\n", RES_ID, Mix_GetError());
		return false;
	}
	return true;
}

bool loadMusicFromResource(music &mus, const char *RES_ID) {
	if (!mus.loadMusic(loadResourceBuffer(RES_ID))) {
		fprintf(stderr, "Could not load music %s: %s\n", RES_ID, Mix_GetError());
		return false;
	}
	return true;
}