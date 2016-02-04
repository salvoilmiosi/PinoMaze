#include "tool_select.h"

void toolSelect::handleEvent(SDL_Event &e) {
    tile *newTile = nullptr;
    int gridX;
    int gridY;

    mazeItem *item;

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        gridX = (e.button.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.button.y - m->clip_rect.y) / m->tileSize;
        currentTile = m->getTile(gridX, gridY);

        selectedTile = nullptr;
        if (currentTile == nullptr) break;

        switch (currentTile->state) {
        case STATE_START:
            state = EDIT_MOVING_START;
            selectedTile = currentTile;
            break;
        case STATE_END:
            state = EDIT_MOVING_END;
            selectedTile = currentTile;
            break;
        case STATE_ITEM:
            state = EDIT_MOVING_ITEM;
            selectedTile = currentTile;
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        state = EDIT_IDLE;
        break;
    case SDL_MOUSEMOTION:
        if (currentTile == nullptr) break;

        gridX = (e.motion.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.motion.y - m->clip_rect.y) / m->tileSize;
        newTile = m->getTile(gridX, gridY);
        if (newTile == nullptr) break;
        if (newTile == currentTile) break;

        switch(state) {
        case EDIT_MOVING_START:
            if (newTile->state == STATE_FLOOR) {
                currentTile->state = STATE_FLOOR;
                newTile->state = STATE_START;
                currentTile = newTile;
                selectedTile = newTile;
            }
            break;
        case EDIT_MOVING_END:
            if (newTile->state == STATE_FLOOR) {
                currentTile->state = STATE_FLOOR;
                newTile->state = STATE_END;
                currentTile = newTile;
                selectedTile = newTile;
            }
            break;
        case EDIT_MOVING_ITEM:
            if (newTile->state == STATE_FLOOR) {
                item = m->findItem(currentTile);
                if (item != nullptr) {
                    m->moveItem(*item, gridX, gridY);
                }

                currentTile = newTile;
                selectedTile = newTile;
            }
            break;
        }
        break;
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_DELETE:
        case SDL_SCANCODE_BACKSPACE:
            if (selectedTile == nullptr) break;

            item = m->findItem(selectedTile);
            if (item != nullptr) {
                m->removeItem(*item);
            }

            selectedTile->state = STATE_FLOOR;
            selectedTile = nullptr;
            break;
        default:
            break;
        }
    }
}

void toolSelect::resetState() {
    state = EDIT_IDLE;
    selectedTile = nullptr;
}

void toolSelect::render(SDL_Renderer *renderer) {
    renderSelection(renderer, selectedTile);
}
