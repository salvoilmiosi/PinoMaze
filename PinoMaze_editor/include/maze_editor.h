#ifndef __MAZE_EDITOR_H__
#define __MAZE_EDITOR_H__

#include <SDL2/SDL.h>

#include <memory>
#include <array>

#include "maze.h"

#include "tool_base.h"

using namespace std;

class mazeEditor {
private:
    static const int TOOLBOX_TIMER = 200;

private:
    maze *m;

    size_t currentTool;
    array<unique_ptr<mazeEditorTool>, 8> tools;

    SDL_Rect clip_rect;

    bool showToolbox;
    int toolboxTimer;

public:
    mazeEditor();

public:
    void setMaze(maze *m);

    void render(SDL_Renderer *renderer, int ox, int oy);

    void tick();

    void handleEvent(SDL_Event &e);

private:
    bool inToolbox(int x, int y);
};

#endif // __MAZE_EDITOR_H__
