#include "tool_wall.h"

#include "resources.h"

#include <cstdlib>

void toolWall::makeWall(bool horizontal, int startPos, int endPos, int wallPos) {
    wall &w = horizontal ? m->hwalls[wallPos] : m->vwalls[wallPos];
    if (endPos > w.size()) {
        endPos = w.size();
    } else if (endPos < 0) {
        endPos = 0;
    }
    if (endPos < startPos) {
        int tmp = startPos;
        startPos = endPos;
        endPos = tmp;
    }
    for (int i=startPos; i<endPos; ++i) {
        w[i] = (state == EDIT_BUILDING) * wallValue;
    }
}

void toolWall::handleEvent(SDL_Event &e) {
    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_MIDDLE) {
            wallValue = wallValue % 3 + 1;
        } else if (inMazeRect(e.button.x, e.button.y)) {
            startX = endX = (e.button.x - m->clip_rect.x + m->tileSize/2) / m->tileSize;
            startY = endY = (e.button.y - m->clip_rect.y + m->tileSize/2) / m->tileSize;
            switch (e.button.button) {
            case SDL_BUTTON_LEFT:
                state = EDIT_BUILDING;
                break;
            case SDL_BUTTON_RIGHT:
                state = EDIT_DESTROYING;
                break;
            }
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (state != EDIT_IDLE) {
            if (abs(endX - startX) > abs(endY - startY)) {
                makeWall(true, startX, endX, startY);
            } else {
                makeWall(false, startY, endY, startX);
            }
        }
        state = EDIT_IDLE;
        break;
    case SDL_MOUSEMOTION:
        if (state != EDIT_IDLE) {
            endX = (e.motion.x - m->clip_rect.x + m->tileSize/2) / m->tileSize;

            endY = (e.motion.y - m->clip_rect.y + m->tileSize/2) / m->tileSize;
        }
        break;
    }
}

void toolWall::render(SDL_Renderer *renderer) {
    if (state != EDIT_IDLE) {
        SDL_Color color = state == EDIT_BUILDING ? COLOR_WALL_BUILD : COLOR_WALL_DESTROY;
        SDL_Rect rect = {
            startX * m->tileSize + m->clip_rect.x - WALL_THICKNESS / 2,
            startY * m->tileSize + m->clip_rect.y - WALL_THICKNESS / 2,
            0, 0
        };
        if (abs(endX - startX) > abs(endY - startY)) {
            rect.w = (endX - startX) * m->tileSize;
            if (rect.w < 0) {
                rect.w = -rect.w;
                rect.x -= rect.w;
            }
            rect.w += WALL_THICKNESS;
            rect.h = WALL_THICKNESS;

            if (rect.x < m->clip_rect.x) {
                rect.w -= m->clip_rect.x - rect.x;
                rect.x = m->clip_rect.x;
            }
            if (rect.x + rect.w > m->clip_rect.x + m->clip_rect.w) {
                rect.w = m->clip_rect.x + m->clip_rect.w - rect.x;
            }
        } else {
            rect.h = (endY - startY) * m->tileSize;
            if (rect.h < 0) {
                rect.h = -rect.h;
                rect.y -= rect.h;
            }
            rect.h += WALL_THICKNESS;
            rect.w = WALL_THICKNESS;

            if (rect.y < m->clip_rect.y) {
                rect.h -= m->clip_rect.y - rect.y;
                rect.y = m->clip_rect.y;
            }
            if (rect.y + rect.h > m->clip_rect.y + m->clip_rect.h) {
                rect.h = m->clip_rect.y + m->clip_rect.h - rect.y;
            }
        }
        SDL_SetRenderDrawColor(renderer, color);
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool toolWall::inMazeRect(int x, int y) {
    return
        x >= m->clip_rect.x && x <= m->clip_rect.x + m->clip_rect.w &&
        y >= m->clip_rect.y && y <= m->clip_rect.y + m->clip_rect.h;
}
