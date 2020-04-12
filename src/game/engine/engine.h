#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL2/SDL.h>

#include <vector>
#include <algorithm>
#include <string>

#include "context.h"
#include "entity.h"

class engine {
public:
    engine(context *con);
    ~engine();

public:
    void mainLoop();

    void tick();
    void render();
    void setStatus(const std::string &status);

    void add_entity(entity *ent) {
        entities.push_back(ent);
    }

    void remove_entity(entity *ent) {
        entities.erase(std::find(entities.begin(), entities.end(), ent));
    }

private:
    float calculate_fps();
    
    context *con;

    std::vector<entity *> entities;
};

#endif // __ENGINE_H__