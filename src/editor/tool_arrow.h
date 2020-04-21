#ifndef __TOOL_ARROW_H__
#define __TOOL_ARROW_H__

#include "tool_base.h"

class toolArrow: public mazeEditorTool {
private:
    mazeItem *item;

    int startX;
    int startY;

public:
    toolArrow(Uint8 sn) : mazeEditorTool(sn) {
        item = nullptr;
    }

public:
    void handleEvent(SDL_Event &e);
};

#endif // __TOOL_ARROW_H__
