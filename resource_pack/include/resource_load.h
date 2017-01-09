#ifndef __RESOURCE_LOAD_H__
#define __RESOURCE_LOAD_H__

#include <SDL2/SDL.h>

bool openResourceFile(const char *filename);

SDL_RWops *getResourceRW(const char *RES_ID);

#endif // __RESOURCES_H__
