#include "engine.h"

#include <iostream>
#include <chrono>

#include <GL/glew.h>
#include <GL/gl.h>

engine::engine(const engine_options &options) : options(options) {
    window = SDL_CreateWindow(options.window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, options.window_width, options.window_height, options.winflags);

    if (!window) {
        throw std::string("Could not create SDL Window: ") + SDL_GetError();
    }

    glcontext = SDL_GL_CreateContext(window);
    if (!glcontext) {
        throw std::string("Could not create OpenGL Context: ") + SDL_GetError();
    }
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetSwapInterval(options.vsync);

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
	if (window) SDL_DestroyWindow(window);
}

void engine::run() {
	SDL_RaiseWindow(window);
	
	SDL_Event event;

	using namespace std::chrono_literals;
	using clock = std::chrono::system_clock;

	static const auto nsPerTick = 1000000000ns / options.tickrate;
	static const auto nsPerFrame = options.fps_limit == 0 ? 0ns : (1000000000ns / options.fps_limit);
	auto lastTick = clock::now();
	auto deltaFrame = lastTick;
	auto lastFrame = lastTick;
	float delta = 0;

	running = true;
	
	while (running) {
		auto now = clock::now();

		delta += (float) std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTick).count() / nsPerTick.count();
		while (delta >= 1) {
			tick();
			--delta;
		}

		lastTick = now;

		if (now - deltaFrame > nsPerFrame) {
			render(std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastFrame).count());
			SDL_GL_SwapWindow(window);
			deltaFrame += nsPerFrame;
			lastFrame = now;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			default:
				handleEvent(event);
				break;
			}
		}
	}
}

void engine::stop() {
	running = false;
}