#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <string>

#include <cmath>
#include <ctime>

#include "engine/engine.h"

#include "renderer.h"

int main (int argc, char **argv) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Could not init SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    context con;

    try {
	    engine m_engine(&con);
        renderer m_renderer(&con);

        m_engine.add_entity(&m_renderer);
        m_engine.mainLoop();
    } catch (const std::string &error) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error.c_str(), nullptr);
        std::cerr << error << std::endl;
        return -2;
    }

	SDL_Quit();

    return 0;
}