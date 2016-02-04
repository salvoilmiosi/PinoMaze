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

    virtual ~toolSelect() {}

public:
    void handleEvent(SDL_Event &e);

    void resetState();

    void render(SDL_Renderer *renderer);
};

#endif // __TOOL_SELECT_H__

