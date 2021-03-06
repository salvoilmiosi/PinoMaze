#ifndef __TOOL_WALL_H__
#define __TOOL_WALL_H__

#include "tool_base.h"

class toolWall: public mazeEditorTool {
private:
    int startX;
    int startY;
    int endX;
    int endY;

    int wallValue = 1;

public:
    toolWall(Uint8 sn) : mazeEditorTool(sn) {}

public:
    void handleEvent(SDL_Event &e) override;

    void render(SDL_Renderer *renderer) override;

private:
    void makeWall(bool horizontal, int startPos, int endPos, int wallPos);

    bool inMazeRect(int x, int y);
};

#endif // __TOOL_WALL_H__
