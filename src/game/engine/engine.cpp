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
			render();
			SDL_GL_SwapWindow(con->window);
			lastFrame += msPerFrame;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			default:
				handleEvent(event);
				break;
			}
		}
	}
}