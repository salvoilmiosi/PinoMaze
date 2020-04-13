#include "engine.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>

engine::engine(context *con) : con(con) {
    Uint32 winflags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    con->window = SDL_CreateWindow(con->window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, con->window_width, con->window_height, winflags);

    if (!con->window) {
        throw std::string("Could not create SDL Window: ") + SDL_GetError();
    }

    SDL_GLContext glcontext = SDL_GL_CreateContext(con->window);
    if (!glcontext) {
        throw std::string("Could not create OpenGL Context: ") + SDL_GetError();
    }
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetSwapInterval(con->vsync);

	glewExperimental = true;
	GLenum error = glewInit();
	if (error != GLEW_OK) {
        throw std::string("Could not init glew: error ") + std::to_string(error);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

engine::~engine() {
	SDL_DestroyWindow(con->window);
}

float engine::calculate_fps() {
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static float prevTicks = SDL_GetTicks();
	static int currentFrame = 0;

	float currentTicks = SDL_GetTicks();
	float frameTime = currentTicks - prevTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = frameTime;

	prevTicks = currentTicks;

	++currentFrame;
	size_t count = (currentFrame < NUM_SAMPLES) ? currentFrame : NUM_SAMPLES;
	float frameTimeAverage = 0;
	for (size_t i=0; i<count; ++i) {
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;
	return 1000.f / frameTimeAverage;
}

void engine::mainLoop() {
	SDL_RaiseWindow(con->window);
	
	SDL_Event event;

	static const float msPerTick = 1000.f / con->tickrate;
	static const float msPerFrame = 1000.f / con->fps_limit;
	float lastTime = SDL_GetTicks();
	float lastFrame = SDL_GetTicks();
	float delta = 0;
	
	bool quit = false;
	while (!quit) {
		int startTicks = SDL_GetTicks();
		delta += (startTicks - lastTime) / msPerTick;
		lastTime = startTicks;
		while (delta >= 1) {
			tick();
			--delta;
		}

		float frameTicks = SDL_GetTicks();
		if (frameTicks - lastFrame > msPerFrame) {
			setStatus(std::to_string((int)calculate_fps()));
			render();
			lastFrame += msPerFrame;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
				break;
			}
		}
	}
}

void engine::tick() {
	for (entity *ent : entities) {
		ent->tick();
	}
}

void engine::render() {
	for (entity *ent : entities) {
		ent->render();
	}
	SDL_GL_SwapWindow(con->window);
}

void engine::setStatus(const std::string &status) {
	for (entity *ent : entities) {
		ent->setStatus(status);
	}
}