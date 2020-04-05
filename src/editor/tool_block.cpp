#include "tool_block.h"

void toolBlock::handleEvent(SDL_Event &e) {
    tile *newTile = nullptr;
    int gridX;
    int gridY;

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        switch (e.button.button) {
        case SDL_BUTTON_LEFT:
            state = EDIT_BUILDING;
            currentTile = nullptr;
            break;
        case SDL_BUTTON_RIGHT:
            state = EDIT_DESTROYING;
            currentTile = nullptr;
            break;
        }
        // fall through
    case SDL_MOUSEMOTION:
        gridX = (e.motion.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.motion.y - m->clip_rect.y) / m->tileSize;
        newTile = m->getTile(gridX, gridY);
        if (newTile == nullptr) break;
        if (newTile == currentTile) break;

        switch (state) {
        case EDIT_BUILDING:
            if (newTile->state == STATE_FLOOR) newTile->state = STATE_BLOCK;
            break;
        case EDIT_DESTROYING:
            if (newTile->state == STATE_BLOCK) newTile->state = STATE_FLOOR;
            break;
        }

        currentTile = newTile;
        break;
    case SDL_MOUSEBUTTONUP:
        state = EDIT_IDLE;
        break;
    }
}
