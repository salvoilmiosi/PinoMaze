#ifndef __TOOL_CROP_H__
#define __TOOL_CROP_H__

#include "tool_base.h"

class toolCrop: public mazeEditorTool {
private:
    int startX;
    int startY;
    int endX;
    int endY;

    static const int BLINK_TIMER = 15;
    int timer;

public:
    toolCrop(Uint8 sn) : mazeEditorTool(sn) {}

    virtual ~toolCrop() {}

public:
    void handleEvent(SDL_Event &e);

    void render(SDL_Renderer *renderer);

    void tick();
};

#endif // __TOOL_CROP_H__
