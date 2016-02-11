#include <windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <cstdio>
#include <memory>

#include "maze_io.h"

#include "game.h"

bool getOpenMazeFile(char *filename) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "Mazes (*.pmz)\0*.pmz\0All files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags |= OFN_NOCHANGEDIR;

	return GetOpenFileName(&ofn) != 0;
}

int main (int argc, char **argv) {
	// Init SDL2 libraries
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
		fprintf(stderr, "Could not init SDL2: %s\n", SDL_GetError());
		return -1;
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		fprintf(stderr, "Could not init png libraries: %s\n", IMG_GetError());
		return -2;
	}

	if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG) {
		fprintf(stderr, "Could not init mp3 libraries: %s\n", Mix_GetError());
		return -2;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
		fprintf(stderr, "Could not init audio libraries: %s\n", Mix_GetError());
		return -3;
	}

	// Load resources
	if (!openResourceFile("resource.dat", "music.dat", "shaders.dat")) {
		fprintf(stderr, "Could not load resources\n");
		return -4;
	}

	unique_ptr<maze> mainMaze = nullptr;

	if (argc > 1) {
		// Use command line arguments for opening a maze
		mainMaze = openMaze(argv[1]);
	} else {
		// Or open the dialog
		char filename[MAX_PATH];
		memset(filename, 0, sizeof(filename));

		if (!getOpenMazeFile(filename)) {
			return 0;
		}

		mainMaze = openMaze(filename);
	}

	if (mainMaze == nullptr) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "PinoMaze", "Cannot open this file", nullptr);
		return 0;
	}

	// Create a window with SDL2
	int winflags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	if (fullscreen) winflags |= SDL_WINDOW_FULLSCREEN;
    SDL_Window *window = SDL_CreateWindow("PinoMaze Game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowWidth, windowHeight, winflags);

	if (window == nullptr) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		return -5;
	}

	SDL_ShowCursor(!fullscreen);

	// Create OpenGL context and Init glew
    SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == nullptr) {
		fprintf(stderr, "Could not create OpenGL context: %s\n", SDL_GetError());
		return -6;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	glewExperimental = true;
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		return false;
	}

	srand(SDL_GetTicks());

	// Create and init the game object
	unique_ptr<game> mainGame = make_unique<game>(mainMaze.get());

	if (!mainGame->init()) {
		fprintf(stderr, "Could not init game\n");
		fgetc(stdin);
		return -7;
	}

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Enable vsync
	SDL_GL_SetSwapInterval(1);

	// Main game loop
	SDL_Event event;

	char fpsCounter[128];
	memset(fpsCounter, 0, 128);

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
			mainGame->tick();

			++ticks;
			--unprocessed;
			shouldRender = true;
		}

		//SDL_Delay(2);

		if (shouldRender) {
			mainGame->render();

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
					mainGame->handleEvent(event);
				}
				break;
			}
		}

		if (SDL_GetTicks() - secondsTimer > 1000) {
			secondsTimer += 1000;
			snprintf(fpsCounter, 128, "Frames: %d\nTicks: %d", frames, ticks);
			mainGame->setStatus(fpsCounter);
			frames = 0;
			ticks = 0;
		}
	}

	// Clean up everything
	mainGame = nullptr;
	mainMaze = nullptr;

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

    return 0;
}
