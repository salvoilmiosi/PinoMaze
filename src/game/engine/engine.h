#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL2/SDL.h>

class engine {
public:
    engine();
    ~engine();

public:
    void tick();
    void render();
    void handleEvent(const SDL_Event &e);
    void setStatus(const char *status);
};

#endif // __ENGINE_H__