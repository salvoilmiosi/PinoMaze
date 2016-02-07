#ifndef __SOUND_H__
#define __SOUND_H__

#include <SDL2/SDL_mixer.h>

class sound {
private:
	Mix_Chunk *chunk = nullptr;
	int channel;

public:
	sound() {}
	sound(Mix_Chunk *chunk, int channel) : chunk(chunk), channel(channel) {}

	virtual ~sound() {
		Mix_FreeChunk(chunk);
	}

	void loadChunk(Mix_Chunk *c, int chan = -1) {
		chunk = c;
		channel = chan;
	}

	bool play(int loops = 0) {
		channel = Mix_PlayChannel(channel, chunk, loops);
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

public:
	music() {}
	music(Mix_Music *mus) : mus(mus) {}

	virtual ~music() {
		Mix_FreeMusic(mus);
	}

	void loadMusic(Mix_Music *m) {
		mus = m;
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