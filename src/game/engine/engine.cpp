#include "engine.h"

#include <iostream>
#include <chrono>

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

void engine::run() {
	SDL_RaiseWindow(con->window);
	
	SDL_Event event;

	using namespace std::chrono_literals;
	using clock = std::chrono::system_clock;

	static const auto nsPerTick = 1000000000ns / con->tickrate;
	static const auto nsPerFrame = 1000000000ns / con->fps_limit;
	auto lastTick = clock::now();
	auto deltaFrame = lastTick;
	auto lastFrame = lastTick;
	float delta = 0;
	
	bool quit = false;
	while (!quit) {
		auto now = clock::now();

		delta += (float) std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTick).count() / nsPerTick.count();
		while (delta >= 1) {
			tick();
			--delta;
		}

		lastTick = now;

		if (now - deltaFrame > nsPerFrame) {
			render(std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastFrame).count());
			SDL_GL_SwapWindow(con->window);
			deltaFrame += nsPerFrame;
			lastFrame = now;
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