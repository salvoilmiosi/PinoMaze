#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <SDL2/SDL.h>

#include "resource_load.h"

extern SDL_Texture *RES_TILES;
extern const int RES_TILES_NUM;
extern int RES_TILES_SIZE;

extern SDL_Texture *RES_TP_FONT;
extern const int RES_TP_FONT_NUM;
extern int RES_TP_FONT_SIZE;

extern SDL_Texture *RES_TOOLS;
extern const int RES_TOOLS_NUM;
extern int RES_TOOLS_SIZE;

extern SDL_Texture *RES_TEXT;
extern const int RES_TEXT_NUM;
extern int RES_TEXT_W;
extern int RES_TEXT_H;

constexpr SDL_Color COLOR_FLOOR =        {0x00, 0x00, 0x00, 0xff};
constexpr SDL_Color COLOR_FLOOR_BLEND =  {0x00, 0x00, 0x00, 0xaa};
constexpr SDL_Color COLOR_NODE_CLOSED =  {0x22, 0x22, 0x00, 0xaa};
constexpr SDL_Color COLOR_NODE_OPEN =    {0x44, 0x44, 0x00, 0xaa};
constexpr SDL_Color COLOR_PATH =         {0x00, 0xff, 0x00, 0xff};
constexpr SDL_Color COLOR_PATH_OLD =     {0x00, 0x80, 0x00, 0xff};
constexpr SDL_Color COLOR_START =        {0xff, 0x00, 0x00, 0xff};
constexpr SDL_Color COLOR_START_TEMP =   {0x66, 0x00, 0x00, 0xff};
constexpr SDL_Color COLOR_END =          {0xff, 0x00, 0xff, 0xff};
constexpr SDL_Color COLOR_END_TEMP =     {0x66, 0x00, 0x66, 0xff};
constexpr SDL_Color COLOR_GRID =         {0xff, 0xff, 0xff, 0x11};
constexpr SDL_Color COLOR_BLOCK =        {0x66, 0x66, 0x66, 0xff};
constexpr SDL_Color COLOR_WALL1 =        {0x99, 0x99, 0x99, 0xff};
constexpr SDL_Color COLOR_WALL2 =        {0x00, 0x99, 0x00, 0xff};
constexpr SDL_Color COLOR_WALL3 =        {0x99, 0x00, 0x99, 0xff};
constexpr SDL_Color COLOR_WALL_BUILD   = {0xff, 0xff, 0xff, 0x99};
constexpr SDL_Color COLOR_WALL_DESTROY = {0xff, 0x00, 0x00, 0x99};
constexpr SDL_Color COLOR_ARROW =        {0xff, 0xff, 0x00, 0xff};
constexpr SDL_Color COLOR_SELECTION =    {0xff, 0xff, 0xff, 0xff};
constexpr SDL_Color COLOR_TELEPORT =     {0xff, 0xff, 0x00, 0xff};
constexpr SDL_Color COLOR_TPTEXT =       {0x00, 0x33, 0xff, 0xff};
constexpr SDL_Color COLOR_TPDEST =       {0xff, 0x00, 0x00, 0xff};
constexpr SDL_Color COLOR_BG =           {0x0a, 0x00, 0x00, 0xff};
constexpr SDL_Color COLOR_CROP_RECT =    {0xff, 0xff, 0xff, 0xff};
constexpr SDL_Color COLOR_CROP_BLINK =   {0xff, 0xff, 0xff, 0x80};
constexpr SDL_Color COLOR_TEXT =         {0xff, 0x00, 0x00, 0xff};

inline Uint32 colorToInt(SDL_PixelFormat *format, SDL_Color color) {
    return SDL_MapRGBA(format, color.r, color.g, color.b, color.a);
}

inline int SDL_SetRenderDrawColor(SDL_Renderer *renderer, const SDL_Color &color) {
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

inline bool operator == (const SDL_Color &a, const SDL_Color &b) {
    return
        (a.r == b.r) &&
        (a.g == b.g) &&
        (a.b == b.b) &&
        (a.a == b.a);
}

bool loadResources(SDL_Renderer *renderer);

#endif // __RESOURCES_H__
