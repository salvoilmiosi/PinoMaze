#ifndef __TOOL_SELECT_H__
#define __TOOL_SELECT_H__

#include "tool_base.h"

class toolSelect: public mazeEditorTool {
private:
    tile *currentTile;
    tile *selectedTile;

public:
    toolSelect(Uint8 sn) : mazeEditorTool(sn) {
        currentTile = nullptr;
    };

public:
    void handleEvent(SDL_Event &e) override;

    void resetState() override;

    void render(SDL_Renderer *renderer) override;
};

#endif // __TOOL_SELECT_H__

