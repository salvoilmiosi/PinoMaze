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
    virtual ~engine();

public:
    void run();

    virtual void tick() = 0;
    virtual void render(float deltaNano) = 0;
    virtual void handleEvent(SDL_Event &event) = 0;

protected:
    bool quit = false;

private:
    context *con;
};

#endif // __ENGINE_H__