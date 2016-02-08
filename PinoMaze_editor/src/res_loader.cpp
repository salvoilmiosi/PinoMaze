#include "res_loader.h"

#include <SDL2/SDL_image.h>

#include <vector>
#include <fstream>

using namespace std;

#include "tile.h"

SDL_Texture *RES_TILES = nullptr;
const int RES_TILES_NUM = 8;
int RES_TILES_SIZE;

SDL_Texture *RES_TP_FONT = nullptr;
const int RES_TP_FONT_NUM = 16;
int RES_TP_FONT_SIZE;

SDL_Texture *RES_TOOLS = nullptr;
const int RES_TOOLS_NUM = 4;
int RES_TOOLS_SIZE;

SDL_Texture *RES_TEXT = nullptr;
const int RES_TEXT_NUM = 16;
int RES_TEXT_W;
int RES_TEXT_H;

const SDL_Color COLOR_FLOOR =        {0x00, 0x00, 0x00, 0xff};
const SDL_Color COLOR_FLOOR_BLEND =  {0x00, 0x00, 0x00, 0xaa};
const SDL_Color COLOR_NODE_CLOSED =  {0x22, 0x22, 0x00, 0xaa};
const SDL_Color COLOR_NODE_OPEN =    {0x44, 0x44, 0x00, 0xaa};
const SDL_Color COLOR_PATH =         {0x00, 0xff, 0x00, 0xff};
const SDL_Color COLOR_PATH_OLD =     {0x00, 0x80, 0x00, 0xff};
const SDL_Color COLOR_START =        {0xff, 0x00, 0x00, 0xff};
const SDL_Color COLOR_START_TEMP =   {0x66, 0x00, 0x00, 0xff};
const SDL_Color COLOR_END =          {0xff, 0x00, 0xff, 0xff};
const SDL_Color COLOR_END_TEMP =     {0x66, 0x00, 0x66, 0xff};
const SDL_Color COLOR_GRID =         {0xff, 0xff, 0xff, 0x11};
const SDL_Color COLOR_BLOCK =        {0x66, 0x66, 0x66, 0xff};
const SDL_Color COLOR_WALL =         {0x99, 0x99, 0x99, 0xff};
const SDL_Color COLOR_WALL_BUILD   = {0xff, 0xff, 0xff, 0x99};
const SDL_Color COLOR_WALL_DESTROY = {0xff, 0x00, 0x00, 0x99};
const SDL_Color COLOR_ARROW =        {0x00, 0x00, 0xff, 0xff};
const SDL_Color COLOR_SELECTION =    {0xff, 0xff, 0xff, 0xff};
const SDL_Color COLOR_TELEPORT =     {0xff, 0xff, 0x00, 0xff};
const SDL_Color COLOR_TPTEXT =       {0x00, 0x33, 0xff, 0xff};
const SDL_Color COLOR_TPDEST =       {0xff, 0x00, 0x00, 0xff};
const SDL_Color COLOR_BG =           {0x0a, 0x00, 0x00, 0xff};
const SDL_Color COLOR_CROP_RECT =    {0xff, 0xff, 0xff, 0xff};
const SDL_Color COLOR_CROP_BLINK =   {0xff, 0xff, 0xff, 0x80};
const SDL_Color COLOR_TEXT =         {0xff, 0x00, 0x00, 0xff};

static const int ID_MAXSIZE = 32;

struct resource {
	size_t size;
	size_t ptr;
	char res_id[ID_MAXSIZE];
};

static vector<resource> resFiles;
static ifstream resourceIfs;

static unsigned int readInt(ifstream &ifs) {
	char data[4];
	ifs.read(data, 4);
	unsigned int num =
		(data[0] << 24 & 0xff000000) |
		(data[1] << 16 & 0x00ff0000) |
		(data[2] << 8 & 0x0000ff00) |
		(data[3] & 0x000000ff);
	return num;
}

bool openResourceFile(const char *filename) {
	resourceIfs.open(filename, ios::binary);

	if (resourceIfs.fail()) {
		return false;
	}

	if (readInt(resourceIfs) != 0x255435f4) {
		return false;
	}

	resFiles.resize(readInt(resourceIfs));

	for (resource &res : resFiles) {
		resourceIfs.read(res.res_id, ID_MAXSIZE);
		res.size = readInt(resourceIfs);
		res.ptr = readInt(resourceIfs);
	}

	return true;
}

void closeResourceFile() {
	resourceIfs.close();
}

static SDL_Surface *loadImageFromResource(const char *RES_ID) {
	for (resource &res : resFiles) {
		if (strcmp(res.res_id, RES_ID) == 0) {
			char *data = new char[res.size];

			resourceIfs.seekg(res.ptr);
			resourceIfs.read(data, res.size);

			return IMG_LoadTyped_RW(SDL_RWFromConstMem(data, res.size), 1, "PNG");
		}
	}
	return nullptr;
}

static void convertPalette(SDL_Surface *surface, const SDL_Color *convertMap, const int ncolors) {
    SDL_LockSurface(surface);

	vector<Uint32> map2(ncolors * 2);
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

static void loadMazeTiles(SDL_Renderer *renderer) {
    const SDL_Color convertMap[] = {
        {0xff, 0xff, 0xff, 0xff}, COLOR_FLOOR_BLEND,
        {0xcc, 0xcc, 0xcc, 0xff}, COLOR_NODE_CLOSED,
        {0x99, 0x99, 0x99, 0xff}, COLOR_NODE_OPEN,
        {0x66, 0x66, 0x66, 0xff}, COLOR_PATH,
        {0x33, 0x33, 0x33, 0xff}, COLOR_PATH_OLD,
        {0x00, 0x00, 0xff, 0xff}, COLOR_ARROW,
        {0x00, 0x00, 0x00, 0xff}, COLOR_WALL,
        {0xff, 0xff, 0x00, 0xff}, COLOR_SELECTION,
        {0xff, 0x00, 0x00, 0xff}, COLOR_TPDEST,
    };

    SDL_Surface *surface = loadImageFromResource("IDB_TILES");
    if (surface != nullptr) {
        convertPalette(surface, convertMap, 9);
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xff, 0x0, 0xff));

        RES_TILES_SIZE = surface->w / RES_TILES_NUM;
        RES_TILES = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

static void loadTeleportTiles(SDL_Renderer *renderer) {
    const SDL_Color convertMap[] = {
        {0xff, 0x00, 0xff, 0xff}, COLOR_TELEPORT,
        {0xff, 0xff, 0xff, 0xff}, COLOR_TPTEXT
    };

    SDL_Surface *surface = loadImageFromResource("IDB_TP_FONT");
    if (surface != nullptr) {
        convertPalette(surface, convertMap, 2);

        RES_TP_FONT_SIZE = surface->w / RES_TP_FONT_NUM;
        RES_TP_FONT = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

static void loadToolTiles(SDL_Renderer *renderer) {
    SDL_Surface *surface = loadImageFromResource("IDB_TOOLS");
    if (surface != nullptr) {
        RES_TOOLS_SIZE = surface->w / RES_TOOLS_NUM;
        RES_TOOLS = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

static void loadTextTiles(SDL_Renderer *renderer) {
    const SDL_Color convertMap[] = {
        {0xff, 0x00, 0xff, 0xff}, {0x00, 0x00, 0x00, 0x00},
        {0xff, 0xff, 0xff, 0xff}, COLOR_TEXT
    };

    SDL_Surface *surface = loadImageFromResource("IDB_TEXT");
    if (surface != nullptr) {
        convertPalette(surface, convertMap, 2);

        RES_TEXT_W = surface->w / RES_TEXT_NUM;
        RES_TEXT_H = surface->h / RES_TEXT_NUM;
        RES_TEXT = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void loadResources(SDL_Renderer *renderer) {
    loadMazeTiles(renderer);
    loadTeleportTiles(renderer);
    loadToolTiles(renderer);
    loadTextTiles(renderer);
}