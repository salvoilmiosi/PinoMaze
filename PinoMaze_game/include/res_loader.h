#ifndef __RES_LOADER_H__
#define __RES_LOADER_H__

#include "../resource.h"

const char *loadStringFromFile(const char *filename);
const char *loadStringFromResource(int RES_ID);

struct SDL_Surface *loadImageFromResources(int RES_ID);
struct Mix_Chunk *loadWaveFromResource(int RES_ID);

#endif
