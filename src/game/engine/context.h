#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <SDL2/SDL.h>
#include <Gl/glew.h>

#include <string>

inline void checkGlError(const char *message) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		throw std::string(message) + ": " + std::string((char*)glewGetErrorString(error));
	}
}

struct context {
    SDL_Window *window = nullptr;
    
    const char *window_title = "GL engine";
    int window_width = 800;
    int window_height = 600;
    int tickrate = 60;
    int fps_limit = 300;
    bool vsync = true;
};

#endif // __CONTEXT_H__