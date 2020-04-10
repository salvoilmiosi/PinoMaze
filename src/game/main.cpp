#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include <ctime>

#include "engine/engine.h"

#include "render/skybox.h"
#include "render/world.h"

#include "maze_io.h"
#include "gui.h"

#include "resources.h"
#include "game.h"

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

	if (openResourceFiles(argv[0], "resource.dat", "music.dat") != 2) {
		std::cerr << "Could not open resource files" << std::endl;
		return -4;
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

    try {
	    engine m_engine(&con);

		game m_game(&con, m_maze.get());
		
		skybox m_skybox(&con, &m_game);
		world m_world(&con, &m_game);

		m_engine.add_entity(&m_game);
		m_engine.add_entity(&m_skybox);
		m_engine.add_entity(&m_world);
		
        m_engine.mainLoop();
    } catch (const std::string &error) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error.c_str(), nullptr);
        std::cerr << error << std::endl;
        return -2;
    }

	SDL_Quit();

    return 0;
}