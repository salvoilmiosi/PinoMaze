#ifndef __MAZE_RENDERER_H__
#define __MAZE_RENDERER_H__

#include <SDL2/SDL.h>

#include "maze.h"

void renderMaze(maze *m, SDL_Renderer *renderer, int ox, int oy);

#endif // __MAZE_RENDERER_H__
