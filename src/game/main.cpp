#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include <ctime>

#include "engine/engine.h"

#include "render/world.h"
#include "render/hud.h"

#include "maze_io.h"
#include "gui.h"

#include "resources.h"
#include "game.h"

class game_engine : public engine {
public:
	game_engine(context *con, maze *m_maze) :
		engine(con),
		m_game(con, m_maze),
		m_world(con, &m_game),
		m_hud(con)
	{}

public:
	void tick() override {
		m_game.tick();
		m_world.tick();
	}

	void render(float deltaNano) override {
		m_game.updateMatrices(deltaNano);
		m_world.render(deltaNano);
		m_hud.render(deltaNano);
	}

	void handleEvent(SDL_Event &event) override {
		if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
			quit = true;
		}
		m_game.handleEvent(event);
	}

private:
	game m_game;
	world m_world;
	hud m_hud;
};

int main (int argc, char **argv) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Could not init SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		fprintf(stderr, "Could not init png libraries: %s\n", IMG_GetError());
		return -2;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
		fprintf(stderr, "Could not init audio libraries: %s\n", Mix_GetError());
		return -3;
	}

	if (!openResourceFiles(argv[0], "resource.dat")) {
		std::cerr << "Could not open resource.dat" << std::endl;
		return -4;
	}

	if (!openResourceFiles(argv[0], "music.dat")) {
		std::cerr << "Could not open music.dat" << std::endl;
	}
    
	std::unique_ptr<maze> m_maze = nullptr;

	if (argc > 1) {
		// Use command line arguments for opening a maze
		m_maze = openMaze(argv[1]);
	} else {
		// Or open the dialog
		const char *filename = getMazeFilename(DIALOG_OPEN);

		if (!filename) {
			return 0;
		}

		m_maze = openMaze(filename);
	}

	if (m_maze == nullptr) {
		messageBox(MESSAGE_WARNING, "Cannot open this file");
		return 0;
	}

    context con;
	con.window_title = "PinoMaze";
	con.vsync = false;
	con.fps_limit = 300;
	con.tickrate = 60;
	//con.window_width = 1920;
	//con.window_height = 1080;
	//con.winflags |= SDL_WINDOW_FULLSCREEN;

	//SDL_ShowCursor(SDL_DISABLE);

    try {
	    game_engine(&con, m_maze.get()).run();
    } catch (const std::string &error) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error.c_str(), nullptr);
        std::cerr << error << std::endl;
        return -2;
    } catch (...) {
		std::cerr << "Generic error" << std::endl;
		return -3;
	}

	SDL_Quit();

    return 0;
}