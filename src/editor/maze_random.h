#ifndef __MAZE_RANDOM_H__
#define __MAZE_RANDOM_H__

#include "maze.h"

#include <memory>

std::unique_ptr<maze> generateRandomMaze(int w, int h);

#endif // __MAZE_RANDOM_H__
