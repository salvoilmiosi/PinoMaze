#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <SDL2/SDL_rwops.h>

struct buffer {
	SDL_RWops *rw = nullptr;
	char *data = nullptr;
	size_t size = 0;

	buffer() {}

	buffer(char *data, size_t size) : data(data), size(size) {
		rw = SDL_RWFromConstMem(data, size);
	}

	virtual ~buffer() {
		if (rw != nullptr) {
			SDL_RWclose(rw);
		}
		if (data != nullptr) {
			delete[]data;
		}
	}
};

#endif