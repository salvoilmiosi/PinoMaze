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

public:
    void handleEvent(SDL_Event &e) override;

    void resetState() override;

    void render(SDL_Renderer *renderer) override;

    void tick() override;
};

#endif // __TOOL_TELEPORT_H__
