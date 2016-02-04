#ifndef __RES_LOADER_H__
#define __RES_LOADER_H__

#include <SDL2/SDL.h>
#include "../resource.h"

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

extern const SDL_Color COLOR_FLOOR;
extern const SDL_Color COLOR_FLOOR_BLEND;
extern const SDL_Color COLOR_NODE_CLOSED;
extern const SDL_Color COLOR_NODE_OPEN;
extern const SDL_Color COLOR_PATH;
extern const SDL_Color COLOR_PATH_OLD;
extern const SDL_Color COLOR_START;
extern const SDL_Color COLOR_START_TEMP;
extern const SDL_Color COLOR_END;
extern const SDL_Color COLOR_END_TEMP;
extern const SDL_Color COLOR_GRID;
extern const SDL_Color COLOR_BLOCK;
extern const SDL_Color COLOR_WALL;
extern const SDL_Color COLOR_WALL_BUILD;
extern const SDL_Color COLOR_WALL_DESTROY;
extern const SDL_Color COLOR_ARROW;
extern const SDL_Color COLOR_SELECTION;
extern const SDL_Color COLOR_TELEPORT;
extern const SDL_Color COLOR_TPTEXT;
extern const SDL_Color COLOR_TPDEST;
extern const SDL_Color COLOR_BG;
extern const SDL_Color COLOR_CROP_RECT;
extern const SDL_Color COLOR_CROP_BLINK;
extern const SDL_Color COLOR_TEXT;

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

void loadResources(SDL_Renderer *renderer);

#endif // __RES_LOADER_H__
