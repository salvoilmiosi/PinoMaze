#ifndef __RES_LOADER_H__
#define __RES_LOADER_H__

#include <SDL2/SDL.h>
#include "../resource.h"

const char *loadStringFromFile(const char *filename);
const char *loadStringFromResource(int RES_ID);

SDL_Surface *loadImageFromResources(int RES_ID);

#endif
