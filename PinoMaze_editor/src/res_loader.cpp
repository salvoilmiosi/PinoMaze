#include "res_loader.h"

#include <SDL2/SDL_image.h>

#include <map>
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
	size_t size = 0;
	size_t ptr = 0;
	const char *filename;
};

map<string, resource> resFiles;

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
	ifstream ifs(filename, ios::binary);

	if (ifs.fail()) {
		return false;
	}

	if (readInt(ifs) != 0x255435f4) {
		return false;
	}

	int numRes = readInt(ifs);

	resource res;
	char res_id[ID_MAXSIZE];

	while (numRes > 0) {
		memset(&res, 0, sizeof(res));
		memset(res_id, 0, sizeof(res_id));

		ifs.read(res_id, ID_MAXSIZE);

		res.size = readInt(ifs);
		res.ptr = readInt(ifs);
		res.filename = filename;

		resFiles[res_id] = res;

		--numRes;
	}

	return true;
}

static SDL_Surface *loadImageFromResource(const char *RES_ID) {
	auto it = resFiles.find(RES_ID);

	if (it != resFiles.end()) {
		resource &res = it->second;
		char *data = new char[res.size];

		ifstream ifs(res.filename, ios::binary);
		ifs.seekg(res.ptr);
		ifs.read(data, res.size);

		SDL_Surface *surf = IMG_LoadTyped_RW(SDL_RWFromConstMem(data, res.size), 1, "PNG");
		return surf;

		delete[]data;
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

static bool loadMazeTiles(SDL_Renderer *renderer) {
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
	if (surface == nullptr)
		return false;

    convertPalette(surface, convertMap, 9);
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

    convertPalette(surface, convertMap, 2);

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

    convertPalette(surface, convertMap, 2);

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