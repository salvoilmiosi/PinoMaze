#ifndef __TOOL_TELEPORT_H__
#define __TOOL_TELEPORT_H__

#include "tool_base.h"

class toolTeleport: public mazeEditorTool {
private:
    static const int FRAME_TIME = 50;

    tile *currentTile;
    tile *selectedTile;

    SDL_Rect tpCharDialRect;

    int timer;

public:
    toolTeleport(Uint8 sn) : mazeEditorTool(sn) {
        currentTile = nullptr;
        selectedTile = nullptr;
        timer = 0;
    }

    virtual ~toolTeleport() {}

public:
    void handleEvent(SDL_Event &e);

    void resetState();

    void render(SDL_Renderer *renderer);

    void tick();
};

#endif // __TOOL_TELEPORT_H__
