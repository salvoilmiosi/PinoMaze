#ifndef __SOUND_H__
#define __SOUND_H__

#include <SDL2/SDL_mixer.h>

class sound {
private:
	Mix_Chunk *chunk;

public:
	sound() : sound(nullptr) {}
	sound(Mix_Chunk *chunk) : chunk(chunk) {}

	virtual ~sound() {
		Mix_FreeChunk(chunk);
	}

	void loadChunk(Mix_Chunk *c) {
		chunk = c;
	}

	void play(int channel = -1, int loops = 0) {
		Mix_PlayChannel(channel, chunk, loops);
	}
};

#endif