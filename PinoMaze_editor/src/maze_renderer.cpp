#include "maze.h"

#include "res_loader.h"

extern int windowWidth;
extern int windowHeight;
#include "tile.h"

#include "res_loader.h"

SDL_Color getTileColor(const tile &t) {
    switch (t.state) {
    case STATE_FLOOR:
    case STATE_ITEM:
        switch (t.temp) {
        case TEMP_NULL: return COLOR_FLOOR_BLEND;
        case TEMP_NODE_CLOSED: return COLOR_NODE_CLOSED;
        case TEMP_NODE_OPEN: return COLOR_NODE_OPEN;
        case TEMP_PATH: return COLOR_PATH;
        case TEMP_PATH_OLD: return COLOR_PATH_OLD;
        case TEMP_START: return COLOR_START_TEMP;
        case TEMP_END: return COLOR_END_TEMP;
        default: return COLOR_FLOOR_BLEND;
        }
    case STATE_BLOCK: return COLOR_BLOCK;
    case STATE_START: return COLOR_START;
    case STATE_END: return COLOR_END;
    default: return COLOR_BG;
    }
}

static void renderWall(maze *m, SDL_Renderer *renderer, const wall &w, bool horizontal, int pos) {
    if (w.isEmpty()) return;

    SDL_SetRenderDrawColor(renderer, COLOR_WALL);

    SDL_Rect rect;
    if (horizontal) {
        rect.x = -WALL_THICKNESS / 2 + m->clip_rect.x;
        rect.y = pos * m->tileSize - WALL_THICKNESS / 2 + m->clip_rect.y;
        rect.w = m->tileSize + WALL_THICKNESS;
        rect.h = WALL_THICKNESS;
    } else {
        rect.x = pos * m->tileSize - WALL_THICKNESS / 2 + m->clip_rect.x;
        rect.y = -WALL_THICKNESS / 2 + m->clip_rect.y;
        rect.w = WALL_THICKNESS;
        rect.h = m->tileSize + WALL_THICKNESS;
    }

    for (int i=0; i<w.length(); ++i) {
        if (rect.x > windowWidth || rect.y > windowHeight)
            break;

        if (w[i] && rect.x + rect.w >= 0 && rect.y + rect.h >= 0) {
            SDL_RenderFillRect(renderer, &rect);
        }
        if (horizontal) {
            rect.x += m->tileSize;
        } else {
            rect.y += m->tileSize;
        }
    }
}

static void renderItem(maze *m, SDL_Renderer *renderer, const mazeItem &item) {
    SDL_Rect srcrect;
    SDL_Rect dstrect = {
        item.item.x * m->tileSize + m->clip_rect.x,
        item.item.y * m->tileSize + m->clip_rect.y,
        m->tileSize, m->tileSize
    };

    if (dstrect.x + dstrect.w < 0 || dstrect.y + dstrect.h < 0)
        return;
    if (dstrect.x > windowWidth || dstrect.y > windowHeight)
        return;

    switch (item.type) {
    case ITEM_TELEPORT:
        srcrect = {
            (item.teleport.tpChar % RES_TP_FONT_NUM) * RES_TP_FONT_SIZE,
            (item.teleport.tpChar / RES_TP_FONT_NUM) * RES_TP_FONT_SIZE,
            RES_TP_FONT_SIZE, RES_TP_FONT_SIZE
        };

        SDL_RenderCopy(renderer, RES_TP_FONT, &srcrect, &dstrect);
        break;
    case ITEM_BRIDGE:
    {
        SDL_Color c = getTileColor(item.bridge.upperTile);
        int state = 0;
        if (c == COLOR_FLOOR_BLEND) {
            state = 0;
        } else if (c == COLOR_NODE_CLOSED) {
            state = 1;
        } else if (c == COLOR_NODE_OPEN) {
            state = 2;
        } else if (c == COLOR_PATH) {
            state = 3;
        } else if (c == COLOR_PATH_OLD) {
            state = 4;
        }
        srcrect = {
            state * RES_TILES_SIZE,
            0, RES_TILES_SIZE, 2 * RES_TILES_SIZE
        };
        dstrect.y -= m->tileSize;
        dstrect.h = m->tileSize * 2;

        SDL_RenderCopy(renderer, RES_TILES, &srcrect, &dstrect);
        break;
    }
    case ITEM_ARROW:
        srcrect = {
            item.arrow.direction * RES_TILES_SIZE,
            2 * RES_TILES_SIZE,
            RES_TILES_SIZE, RES_TILES_SIZE
        };

        SDL_RenderCopy(renderer, RES_TILES, &srcrect, &dstrect);
        break;
    }
}

void renderMaze(maze *m, SDL_Renderer *renderer, int ox, int oy) {
    m->clip_rect = {ox, oy, m->tileSize * m->width(), m->tileSize * m->height()};

    SDL_SetRenderDrawColor(renderer, COLOR_FLOOR);
    SDL_RenderFillRect(renderer, (SDL_Rect *)&m->clip_rect);

	for (int i = 0; i < m->datasize(); ++i) {
		tile *t = m->getTile(i);

        SDL_Rect rect = {
            i % m->width() * m->tileSize + ox,
            i / m->width() * m->tileSize + oy,
            m->tileSize, m->tileSize
        };

        if (rect.x + rect.w < 0 || rect.y + rect.h < 0)
            continue;
        if (rect.x > windowWidth || rect.y > windowHeight)
            continue;

        SDL_SetRenderDrawColor(renderer, getTileColor(*t));
        SDL_RenderFillRect(renderer, &rect);
	}

    if (m->renderGrid) {
        SDL_SetRenderDrawColor(renderer, COLOR_GRID);
        for (int x=0; x<=m->width(); ++x) {
            SDL_RenderDrawLine(renderer,
                x * m->tileSize + ox, oy,
                x * m->tileSize + ox, oy + m->height() * m->tileSize);
        }
        for (int y=0; y<=m->height(); ++y) {
            SDL_RenderDrawLine(renderer,
                ox, y * m->tileSize + oy,
                ox + m->width() * m->tileSize, y * m->tileSize + oy);
        }
    }

    int i = 0;
    for (const wall &it : m->hwalls) {
        renderWall(m, renderer, it, true, i++);
    }

    i = 0;
    for (const wall &it : m->vwalls) {
        renderWall(m, renderer, it, false, i++);
    }

    for (pair<const int, mazeItem> &it : m->items) {
        renderItem(m, renderer, it.second);
    }
}

