#ifndef __SOUND_H__
#define __SOUND_H__

#include <SDL2/SDL_mixer.h>

#include "buffer.h"

#include <memory>

using namespace std;

class sound {
private:
	Mix_Chunk *chunk = nullptr;
	int channel;

public:
	virtual ~sound() {
		if (chunk != nullptr) {
			Mix_FreeChunk(chunk);
		}
	}

	bool loadChunk(unique_ptr<buffer> b, int chan = -1) {
		if (!b || b->rw == nullptr) {
			return false;
		}
		Mix_Chunk *c = Mix_LoadWAV_RW(b->rw, 0);
		if (c == nullptr) {
			return false;
		}
		if (chunk != nullptr && chunk != c) {
			Mix_FreeChunk(chunk);
		}
		chunk = c;
		return true;
	}

	bool play(int loops = 0, int chan = -1) {
		channel = Mix_PlayChannel(chan, chunk, loops);
		return channel >= 0;
	}

	void pause() {
		Mix_Pause(channel);
	}

	void resume() {
		Mix_Resume(channel);
	}

	void stop() {
		Mix_HaltChannel(channel);
	}

	void volume(float vol) {
		Mix_Volume(channel, int(vol * MIX_MAX_VOLUME));
	}
};

class music {
private:
	Mix_Music *mus = nullptr;
	unique_ptr<buffer> buf;

public:
	virtual ~music() {
		if (mus != nullptr) {
			//stop();
			Mix_FreeMusic(mus);
		}
	}

	bool loadMusic(unique_ptr<buffer> b) {
		if (!b || b->rw == nullptr) {
			return false;
		}
		Mix_Music *m = Mix_LoadMUSType_RW(b->rw, MUS_OGG, 0);
		if (m == nullptr) {
			return false;
		}
		if (mus != nullptr && mus != m) {
			stop();
			Mix_FreeMusic(mus);
		}
		mus = m;
		buf = move(b);
		return true;
	}

	bool play(int loops = -1) {
		return Mix_PlayMusic(mus, loops) >= 0;
	}

	void pause() {
		Mix_PauseMusic();
	}

	void resume() {
		Mix_ResumeMusic();
	}

	void stop() {
		Mix_HaltMusic();
	}

	void volume(float vol) {
		Mix_VolumeMusic(int(vol * MIX_MAX_VOLUME));
	}

	bool fadeIn(int ms, int loops = -1) {
		return Mix_FadeInMusic(mus, loops, ms) >= 0;
	}

	void fadeOut(int ms) {
		Mix_FadeOutMusic(ms);
	}
};

#endif