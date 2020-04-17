#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL2/SDL.h>

#include <vector>
#include <algorithm>
#include <string>

#include "context.h"

class engine {
public:
    engine(context *con);
    ~engine();

public:
    void mainLoop();

    virtual void tick() = 0;
    virtual void render(float deltaMs) = 0;
    virtual void handleEvent(SDL_Event &event) = 0;

private:
    context *con;
};

#endif // __ENGINE_H__