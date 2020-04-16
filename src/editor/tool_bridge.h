#ifndef __TOOL_BRIDGE_H__
#define __TOOL_BRIDGE_H__

#include "tool_base.h"

class toolBridge: public mazeEditorTool {
private:
    tile *currentTile;
    tile *selectedTile;

    int wallValue = 1;

public:
    toolBridge(Uint8 sn) : mazeEditorTool(sn) {
        currentTile = nullptr;
        selectedTile = nullptr;
    }

    virtual ~toolBridge() {}

public:
    void handleEvent(SDL_Event &e);

    void resetState();

    void render(SDL_Renderer *renderer);
};

#endif // __TOOL_BRIDGE_H__
