#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

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

struct engine_options {
    Uint32 winflags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    const char *window_title = "GL engine";
    int window_width = 800;
    int window_height = 600;
    int tickrate = 60;
    int fps_limit = 300;
    bool vsync = true;
    float fov = 90.f;
};

class engine {
public:
    engine(const engine_options &options);
    virtual ~engine();

public:
    void run();

    virtual void tick() = 0;
    virtual void render(float deltaNano) = 0;
    virtual void handleEvent(SDL_Event &event) = 0;

private:
    SDL_Window *window = nullptr;
    SDL_GLContext glcontext;

protected:
    bool running = false;

public:
    engine_options options;
};

#endif // __ENGINE_H__