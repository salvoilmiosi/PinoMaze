#include "resources.h"

#include <SDL2/SDL_image.h>

#include <map>
#include <vector>
#include <fstream>

using namespace std;

#include "tile.h"

SDL_Texture *RES_TILES = nullptr;
const int RES_TILES_NUM = 8;
int RES_TILES_SIZE = 0;

SDL_Texture *RES_TP_FONT = nullptr;
const int RES_TP_FONT_NUM = 16;
int RES_TP_FONT_SIZE = 0;

SDL_Texture *RES_TOOLS = nullptr;
const int RES_TOOLS_NUM = 4;
int RES_TOOLS_SIZE = 0;

SDL_Texture *RES_TEXT = nullptr;
const int RES_TEXT_NUM = 16;
int RES_TEXT_W = 0;
int RES_TEXT_H = 0;

static SDL_Surface *loadImageFromResource(const char *RES_ID) {
    SDL_RWops *rw = getResourceRW(RES_ID);
    if (!rw) return nullptr;

    return IMG_LoadTyped_RW(rw, 1, "PNG");
}

static void convertPalette(SDL_Surface *surface, const SDL_Color *convertMap, const int ncolors) {
    SDL_LockSurface(surface);

	std::vector<Uint32> map2(ncolors * 2);
    for (int i=0; i<ncolors*2; ++i) {
        map2[i] = colorToInt(surface->format, convertMap[i]);
    }

    const Uint8 bpp = surface->format->BytesPerPixel;
    if (bpp != 3 && bpp != 4) return;

    Uint8 *pixels = (Uint8 *)surface->pixels;
    for (Uint8 *p = pixels; p-pixels < surface->w*surface->h*bpp; p+=bpp) {
        Uint32 color = *((Uint32*)p);
        if (bpp == 3) {
            #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            color = (color & 0xffffff00) >> 8;
            #else
            color &= 0x00ffffff;
            #endif
        }
        for (int j=0; j < ncolors; ++j) {
            if (color == map2[j*2]) {
                color = map2[j*2+1];
                if (bpp == 4) {
                    *((Uint32 *)p) = color;
                } else if (bpp == 3) {
                    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    p[0] = (color & 0xff0000) >> 16;
                    p[1] = (color & 0x00ff00) >> 8;
                    p[2] = (color & 0x0000ff);
                    #else
                    p[0] = (color & 0x0000ff);
                    p[1] = (color & 0x00ff00) >> 8;
                    p[2] = (color & 0xff0000) >> 16;
                    #endif
                }
                break;
            }
        }
    }

    SDL_UnlockSurface(surface);
}

static bool loadMazeTiles(SDL_Renderer *renderer) {
    const SDL_Color convertMap[] = {
        {0xff, 0xff, 0xff, 0xff}, COLOR_FLOOR_BLEND,
        {0xcc, 0xcc, 0xcc, 0xff}, COLOR_NODE_CLOSED,
        {0x99, 0x99, 0x99, 0xff}, COLOR_NODE_OPEN,
        {0x66, 0x66, 0x66, 0xff}, COLOR_PATH,
        {0x33, 0x33, 0x33, 0xff}, COLOR_PATH_OLD,
        {0x00, 0x00, 0xff, 0xff}, COLOR_ARROW,
        {0x00, 0x00, 0x00, 0xff}, COLOR_WALL1,
        {0x00, 0xff, 0x00, 0xff}, COLOR_WALL2,
        {0x00, 0x80, 0xff, 0xff}, COLOR_WALL3,
        {0xff, 0xff, 0x00, 0xff}, COLOR_SELECTION,
        {0xff, 0x00, 0x00, 0xff}, COLOR_TPDEST,
    };

    SDL_Surface *surface = loadImageFromResource("IDB_TILES");
	if (surface == nullptr)
		return false;

    convertPalette(surface, convertMap, sizeof(convertMap) / sizeof(SDL_Color));
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xff, 0x0, 0xff));

    RES_TILES_SIZE = surface->w / RES_TILES_NUM;
    RES_TILES = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

	return true;
}

static bool loadTeleportTiles(SDL_Renderer *renderer) {
    const SDL_Color convertMap[] = {
        {0xff, 0x00, 0xff, 0xff}, COLOR_TELEPORT,
        {0xff, 0xff, 0xff, 0xff}, COLOR_TPTEXT
    };

    SDL_Surface *surface = loadImageFromResource("IDB_TP_FONT");
	if (surface == nullptr)
		return false;

    convertPalette(surface, convertMap, sizeof(convertMap) / sizeof(SDL_Color));

    RES_TP_FONT_SIZE = surface->w / RES_TP_FONT_NUM;
    RES_TP_FONT = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

	return true;
}

static bool loadToolTiles(SDL_Renderer *renderer) {
    SDL_Surface *surface = loadImageFromResource("IDB_TOOLS");
	if (surface == nullptr)
		return false;

    RES_TOOLS_SIZE = surface->w / RES_TOOLS_NUM;
    RES_TOOLS = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

	return true;
}

static bool loadTextTiles(SDL_Renderer *renderer) {
    const SDL_Color convertMap[] = {
        {0xff, 0x00, 0xff, 0xff}, {0x00, 0x00, 0x00, 0x00},
        {0xff, 0xff, 0xff, 0xff}, COLOR_TEXT
    };

    SDL_Surface *surface = loadImageFromResource("IDB_TEXT");
	if (surface == nullptr)
		return false;

    convertPalette(surface, convertMap, sizeof(convertMap) / sizeof(SDL_Color));

    RES_TEXT_W = surface->w / RES_TEXT_NUM;
    RES_TEXT_H = surface->h / RES_TEXT_NUM;
    RES_TEXT = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

	return true;
}

bool loadResources(SDL_Renderer *renderer) {
	if (!loadMazeTiles(renderer)) return false;
	if (!loadTeleportTiles(renderer)) return false;
	if (!loadToolTiles(renderer)) return false;
	if (!loadTextTiles(renderer)) return false;

	return true;
}