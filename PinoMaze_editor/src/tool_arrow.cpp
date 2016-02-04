#include "tool_arrow.h"

#include <cstdlib>

void toolArrow::handleEvent(SDL_Event &e) {
    tile *t;

    int gridX;
    int gridY;

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        startX = e.button.x;
        startY = e.button.y;
        gridX = (startX - m->clip_rect.x) / m->tileSize;
        gridY = (startY - m->clip_rect.y) / m->tileSize;
        t = m->getTile(gridX, gridY);

        if (t == nullptr) break;

        if (t->state == STATE_FLOOR) {
            mazeItem a = makeItem(ITEM_ARROW);
            a.arrow.x = gridX;
            a.arrow.y = gridY;
            m->addItem(a);
        }
        item = m->findItem(t);
        if (item != nullptr) {
            state = EDIT_BUILDING;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        state = EDIT_IDLE;
        break;
    case SDL_MOUSEMOTION:
        if (state == EDIT_BUILDING && item != nullptr) {
            if (abs(e.motion.x - startX) > abs(e.motion.y - startY)) {
                if (e.motion.x > startX) {
                    item->arrow.direction = ARROW_RIGHT;
                } else {
                    item->arrow.direction = ARROW_LEFT;
                }
            } else {
                if (e.motion.y > startY) {
                    item->arrow.direction = ARROW_DOWN;
                } else {
                    item->arrow.direction = ARROW_UP;
                }
            }
        }
    }
}

