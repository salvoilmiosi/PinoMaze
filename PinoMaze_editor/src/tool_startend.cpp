#include "tool_startend.h"

void toolStartend::handleEvent(SDL_Event &e) {
    tile *oldTile = nullptr;
    tile *newTile = nullptr;

    int gridX;
    int gridY;

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        gridX = (e.button.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.button.y - m->clip_rect.y) / m->tileSize;
        if (m->getTile(gridX, gridY) == nullptr) break;

        if (e.button.button == SDL_BUTTON_LEFT) {
            state = EDIT_MOVING_START;
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
            state = EDIT_MOVING_END;
        }
        // fall through
    case SDL_MOUSEMOTION:
        gridX = (e.motion.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.motion.y - m->clip_rect.y) / m->tileSize;
        newTile = m->getTile(gridX, gridY);

        if (newTile == nullptr) break;
        if (newTile == oldTile) break;

        switch (state) {
        case EDIT_MOVING_START:
            oldTile = m->startTile();
            if (newTile->state == STATE_FLOOR) {
                if (oldTile) {
                    oldTile->state = STATE_FLOOR;
                }
                newTile->state = STATE_START;
            }
            break;
        case EDIT_MOVING_END:
            oldTile = m->endTile();
            if (newTile->state == STATE_FLOOR) {
                if (oldTile) {
                    oldTile->state = STATE_FLOOR;
                }
                newTile->state = STATE_END;
            }
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        state = EDIT_IDLE;
        break;
    }
}
