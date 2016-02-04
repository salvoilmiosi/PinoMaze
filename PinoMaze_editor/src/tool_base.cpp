#include "tool_base.h"

#include "res_loader.h"

void mazeEditorTool::renderSelection(SDL_Renderer *renderer, tile *selectedTile) {
    if (selectedTile == nullptr) return;

    int i = m->getIndex(selectedTile);
    int x = i % m->width();
    int y = i / m->width();

    SDL_Rect srcrect = {
        0, 3 * RES_TILES_SIZE,
        2 * RES_TILES_SIZE, 2 * RES_TILES_SIZE
    };
    SDL_Rect dstrect = {
        x * m->tileSize + m->clip_rect.x - 2 * m->tileSize / RES_TILES_SIZE,
        y * m->tileSize + m->clip_rect.y - 2 * m->tileSize / RES_TILES_SIZE,
        m->tileSize * 2, m->tileSize * 2
    };
    SDL_RenderCopy(renderer, RES_TILES, &srcrect, &dstrect);
}
