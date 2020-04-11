#ifndef __SOUND_H__
#define __SOUND_H__

#include <SDL2/SDL_mixer.h>

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

	bool loadChunk(SDL_RWops *rw, int chan = -1) {
		if (!rw) {
			return false;
		}
		Mix_Chunk *c = Mix_LoadWAV_RW(rw, 1);
		if (!c) {
			return false;
		}
		if (chunk && chunk != c) {
			Mix_FreeChunk(chunk);
		}
		chunk = c;
		return true;
	}

	bool play(int loops = 0, int chan = -1) {
		return true;
		// channel = Mix_PlayChannel(chan, chunk, loops);
		// return channel >= 0;
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
	SDL_RWops *rw_buf = nullptr;

public:
	virtual ~music() {
		if (mus) {
			Mix_FreeMusic(mus);
		}

		if (rw_buf) {
			SDL_RWclose(rw_buf);
		}
	}

	bool loadMusic(SDL_RWops *rw) {
		if (!rw) {
			return false;
		}
		Mix_Music *m = Mix_LoadMUSType_RW(rw, MUS_OGG, 0);
		if (!m) {
			return false;
		}
		if (mus && mus != m) {
			stop();
			Mix_FreeMusic(mus);
			if (rw_buf) {
				SDL_RWclose(rw_buf);
			}
		}
		mus = m;
		rw_buf = rw;
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