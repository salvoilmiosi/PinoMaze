#ifndef __TOOL_BLOCK_H__
#define __TOOL_BLOCK_H__

#include "tool_base.h"

class toolBlock: public mazeEditorTool {
private:
    tile *currentTile;

public:
    toolBlock(Uint8 sn) : mazeEditorTool(sn) {}

    virtual ~toolBlock() {}

public:
    void handleEvent(SDL_Event &e);
};

#endif // __TOOL_BLOCK_H__
