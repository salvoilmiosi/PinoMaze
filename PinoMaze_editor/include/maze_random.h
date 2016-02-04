#ifndef __MAZE_RANDOM_H__
#define __MAZE_RANDOM_H__

#include "maze.h"

#include <memory>

static const int BLOCK_PROBABILITY = 10;
static const int WALL_PROBABILITY_H = 15;
static const int WALL_PROBABILITY_V = 15;

unique_ptr<maze> generateRandomMaze(int w, int h);

#endif // __MAZE_RANDOM_H__
