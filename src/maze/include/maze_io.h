#ifndef __MAZE_IO_H__
#define __MAZE_IO_H__

#include "maze.h"

#include <string>
#include <memory>

static const unsigned int MAGIC_NUMBER = 0xCDC1DAC5;
static const unsigned int VERSION_NUMBER = 0x01000000;

void saveMaze(const char *filename, maze *m);

std::unique_ptr<maze> openMaze(const char *filename);

#endif // __MAZE_IO_H__
