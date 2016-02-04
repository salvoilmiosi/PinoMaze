#ifndef __TOOL_BASE_H__
#define __TOOL_BASE_H__

#include <SDL2/SDL.h>

#include "maze.h"

class mazeEditorTool {
protected:
    static const Uint8 EDIT_IDLE = 0;
    static const Uint8 EDIT_BUILDING = 1;
    static const Uint8 EDIT_DESTROYING = 2;
    static const Uint8 EDIT_MOVING_START = 3;
    static const Uint8 EDIT_MOVING_END = 4;
    static const Uint8 EDIT_MOVING_ITEM = 5;
    static const Uint8 EDIT_MOVING_WARP = 6;
    static const Uint8 EDIT_TPCHAR = 7;
    static const Uint8 EDIT_CROP = 8;

    Uint8 state;

    maze *m;

private:
    const Uint8 sn;

public:
    mazeEditorTool(Uint8 sn) : sn(sn) {
        state = EDIT_IDLE;
        m = nullptr;
    }

    virtual ~mazeEditorTool() {}

public:
    void setMaze(maze *_m) {
        m = _m;
    }

    Uint8 spriteNum() const {
        return sn;
    }

public:
    virtual void handleEvent(SDL_Event &e) = 0;

    virtual void resetState() {
        state = EDIT_IDLE;
    }

    virtual void render(SDL_Renderer *renderer) {};

    virtual void tick() {};

protected:
    void renderSelection(SDL_Renderer *renderer, tile *selectedTile);
};

#endif // __TOOL_BASE_H__
