#ifndef __MAZE_EDITOR_H__
#define __MAZE_EDITOR_H__

#include <SDL2/SDL.h>

#include "maze.h"

#include "tool_base.h"

class mazeEditor {
private:
    static const Uint8 TOOLBOX_TIMER = 200;

    static const Uint8 TOOL_COUNT = 8;

private:
    maze *m;

    Uint8 currentTool;
    mazeEditorTool *tools[TOOL_COUNT];

    SDL_Rect clip_rect;

    bool showToolbox;
    Uint8 toolboxTimer;

public:
    mazeEditor();
    virtual ~mazeEditor();

public:
    void setMaze(maze *m);

    void render(SDL_Renderer *renderer, int ox, int oy);

    void tick();

    void handleEvent(SDL_Event &e);

private:
    bool inToolbox(int x, int y);
};

#endif // __MAZE_EDITOR_H__
