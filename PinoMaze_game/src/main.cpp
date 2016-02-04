#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <cstdio>
#include <memory>

#include "maze_io.h"

#include "game.h"

namespace {
	SDL_Window *window;

	unique_ptr<game> mainGame = nullptr;
	unique_ptr<maze> mainMaze = nullptr;
}

static unique_ptr<maze> openMazeFile() {
    char filename[MAX_PATH];
    memset(filename, 0, sizeof(filename));

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

    if (GetOpenFileName(&ofn)) {
        unique_ptr<maze> m = openMaze(filename);
        if (m == nullptr) {
            MessageBox(nullptr, "Cannot open this file", "Error", MB_ICONERROR);
        } else {
            return m;
        }
    }
    return nullptr;
}

bool init() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    glewExperimental = true;
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		return false;
	}

	mainGame.reset(new game);

    mainGame->setMaze(mainMaze.get());

    if (!mainGame->init()) return false;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    SDL_GL_SetSwapInterval(0);

    return true;
}

void clean() {
    SDL_DestroyWindow(window);
	IMG_Quit();
    SDL_Quit();
}

void tick() {
    mainGame->tick();
}

void render() {
    mainGame->render();

    SDL_GL_SwapWindow(window);
}

void mainLoop() {
	SDL_Event event;

	int lastTime = SDL_GetTicks();
	float unprocessed = 0;
	float msPerTick = 1000.f / tickrate;
	int frames = 0;
	int ticks = 0;
	int secondsTimer = SDL_GetTicks();

	char fpsCounter[128];
	memset(fpsCounter, 0, 128);

    while(true) {
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

        //SDL_Delay(2);

        if (shouldRender) {
            render();
            ++frames;
        }

        while(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    return;
				// fall through
			default:
				mainGame->handleEvent(event);
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
}

int main (int argc, char **argv) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		MessageBox(nullptr, "Could not init SDL2", "Error", MB_ICONERROR);
		return -1;
	}
	if (!IMG_Init(IMG_INIT_PNG) != 0) {
		MessageBox(nullptr, "Could not init png libraries", "Error", MB_ICONERROR);
		return -2;
	}

	mainMaze = openMazeFile();
    if (mainMaze == nullptr) return 0;

    window = SDL_CreateWindow("PinoMaze Game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		MessageBox(nullptr, "Coult not create window", "Error", MB_ICONERROR);
		return -3;
	}

    SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!init()) {
		MessageBox(nullptr, "Could not init game", "Error", MB_ICONERROR);
		return -4;
	}

	mainLoop();

	mainGame.release();

    clean();
    SDL_GL_DeleteContext(context);

    return 0;
}
