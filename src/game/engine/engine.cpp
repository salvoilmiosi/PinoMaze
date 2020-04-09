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

void engine::mainLoop() {
	SDL_Event event;

	std::string fpsCounter;

	int lastTime = SDL_GetTicks();
	float unprocessed = 0;
	float msPerTick = 1000.f / con->tickrate;
	int frames = 0;
	int ticks = 0;
	int secondsTimer = SDL_GetTicks();

	bool quit = false;
	while (!quit) {
		int now = SDL_GetTicks();
		unprocessed += (now - lastTime) / msPerTick;
		lastTime = now;
		bool shouldRender = false;
		while (unprocessed >= 1) {
			tick();

			++ticks;
			--unprocessed;
			shouldRender = true;
		}

		if (shouldRender) {
			render();

			SDL_GL_SwapWindow(con->window);

			++frames;
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

		if (SDL_GetTicks() - secondsTimer > 1000) {
			secondsTimer += 1000;
			fpsCounter = std::string(con->window_title) + " Frames: " + std::to_string(frames) + " Ticks: " + std::to_string(ticks);
			SDL_SetWindowTitle(con->window, fpsCounter.c_str());
			frames = 0;
			ticks = 0;
		}
	}
}

void engine::tick() {
	for (entity *ent : entities) {
		ent->tick();
	}
}

void engine::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (entity *ent : entities) {
		ent->render();
	}
}