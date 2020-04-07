#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <string>

#include <cmath>
#include <ctime>

#include "globals.h"

#include "engine/engine.h"

void gameLoop(SDL_Window *window) {
    engine m_engine;

	SDL_Event event;

	std::string fpsCounter;

	int lastTime = SDL_GetTicks();
	float unprocessed = 0;
	float msPerTick = 1000.f / tickrate;
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
			m_engine.tick();

			++ticks;
			--unprocessed;
			shouldRender = true;
		}

		//SDL_Delay(2);

		if (shouldRender) {
			m_engine.render();

			SDL_GL_SwapWindow(window);

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
				} else {
					m_engine.handleEvent(event);
				}
				break;
			}
		}

		if (SDL_GetTicks() - secondsTimer > 1000) {
			secondsTimer += 1000;
			fpsCounter = "Frames: " + std::to_string(frames) + "\nTicks: " + std::to_string(ticks);
			m_engine.setStatus(fpsCounter.c_str());
			frames = 0;
			ticks = 0;
		}
	}
}

int main (int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Could not init SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Uint32 winflags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    SDL_Window *window = SDL_CreateWindow("PinoMaze game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, winflags);

    if (!window) {
        std::cerr << "Could not create SDL Window: " << SDL_GetError() << std::endl;
        return -2;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "Could not create OpenGL Context: " << SDL_GetError() << std::endl;
        return -3;
    }
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	glewExperimental = true;
	GLenum error = glewInit();
	if (error != GLEW_OK) {
        std::cerr << "Could not init glew: error " << error << std::endl;
		return -4;
	}

    srand(time(NULL));

    
	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Enable vsync
	SDL_GL_SetSwapInterval(1);

    gameLoop(window);

    return 0;
}