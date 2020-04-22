#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <SDL2/SDL.h>
#include <Gl/glew.h>

#include <string>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

inline void checkGlError(const char *message) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		throw std::string(message) + ": " + std::string((char*)glewGetErrorString(error));
	}
}

static auto MAX = [](auto a, auto b) {
	return a > b ? a : b;
};

static auto MIN = [](auto a, auto b) {
	return a > b ? b : a;
};
struct context {
    SDL_Window *window = nullptr;
    Uint32 winflags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    const char *window_title = "GL engine";
    int window_width = 800;
    int window_height = 600;
    int tickrate = 60;
    int fps_limit = 300;
    bool vsync = true;
    float fov = 90.f;
};

#endif // __CONTEXT_H__