#ifndef __TOOL_BRIDGE_H__
#define __TOOL_BRIDGE_H__

#include "tool_base.h"

class toolBridge: public mazeEditorTool {
private:
    tile *currentTile;
    tile *selectedTile;

public:
    toolBridge(Uint8 sn) : mazeEditorTool(sn) {
        currentTile = nullptr;
        selectedTile = nullptr;
    }

public:
    void handleEvent(SDL_Event &e) override;

    void resetState() override;

    void render(SDL_Renderer *renderer) override;
};

#endif // __TOOL_BRIDGE_H__
