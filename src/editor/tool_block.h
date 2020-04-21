#ifndef __TOOL_BLOCK_H__
#define __TOOL_BLOCK_H__

#include "tool_base.h"

class toolBlock: public mazeEditorTool {
private:
    tile *currentTile;

public:
    toolBlock(Uint8 sn) : mazeEditorTool(sn) {}

public:
    void handleEvent(SDL_Event &e) override;
};

#endif // __TOOL_BLOCK_H__
