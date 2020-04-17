#include "tool_bridge.h"

#include "resources.h"

void toolBridge::handleEvent(SDL_Event &e) {
    short gridX;
    short gridY;

    tile *newTile = nullptr;

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        gridX = (e.button.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.button.y - m->clip_rect.y) / m->tileSize;
        currentTile = m->getTile(gridX, gridY);

        if (currentTile == nullptr) break;

        if (e.button.button == SDL_BUTTON_LEFT) {
            if (currentTile->state == STATE_FLOOR) {
                mazeItem b = makeItem(ITEM_BRIDGE);
                b.bridge.x = gridX;
                b.bridge.y = gridY;
                m->addItem(b);
            }

            if (currentTile->state == STATE_ITEM) {
                state = EDIT_BUILDING;
                selectedTile = currentTile;
            }
        } else if (e.button.button == SDL_BUTTON_MIDDLE) {
            mazeItem *item = m->findItem(currentTile);
            if (item && item->type == ITEM_BRIDGE) {
                item->bridge.wallLower = (item->bridge.wallLower + 1) % 3;
                item->bridge.wallUpper = (item->bridge.wallUpper + 1) % 3;
            }
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

        if (state == EDIT_BUILDING && newTile->state == STATE_FLOOR) {
            mazeItem *item = m->findItem(currentTile);

            if (item != nullptr) {
                m->moveItem(*item, gridX, gridY);
                selectedTile = newTile;
                currentTile = newTile;
            }
        }

        break;
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_DELETE:
        case SDL_SCANCODE_BACKSPACE:
            if (selectedTile == nullptr) break;

            if (mazeItem *item = m->findItem(selectedTile)) {
                m->removeItem(*item);
            }

            selectedTile = nullptr;
            break;
        default:
            break;
        }
        break;
    }
}

void toolBridge::resetState() {
    state = EDIT_IDLE;
    selectedTile = nullptr;
}

void toolBridge::render(SDL_Renderer *renderer) {
    renderSelection(renderer, selectedTile);
}
