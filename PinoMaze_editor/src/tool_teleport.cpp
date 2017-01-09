#include "tool_teleport.h"

#include "resources.h"

extern int windowWidth;
extern int windowHeight;

void toolTeleport::handleEvent(SDL_Event &e) {
    short gridX;
    short gridY;

    tile *newTile = nullptr;
    mazeItem *item = nullptr;

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        switch(state) {
        case EDIT_TPCHAR:
            if (e.button.button == SDL_BUTTON_MIDDLE) {
                state = EDIT_IDLE;
                break;
            }

            if (e.button.x >= tpCharDialRect.x && e.button.x < tpCharDialRect.x + tpCharDialRect.w &&
                    e.button.y >= tpCharDialRect.y && e.button.y < tpCharDialRect.y + tpCharDialRect.h) {
                item = m->findItem(selectedTile);

                if (item != nullptr) {
                    int x = (e.button.x - tpCharDialRect.x) / RES_TP_FONT_SIZE;
                    int y = (e.button.y - tpCharDialRect.y) / RES_TP_FONT_SIZE;
                    item->teleport.tpChar = y * RES_TP_FONT_NUM + x;

                    state = EDIT_IDLE;
                    break;
                }
            }
            state = EDIT_IDLE;
            // fall through
        case EDIT_IDLE:
            gridX = (e.button.x - m->clip_rect.x) / m->tileSize;
            gridY = (e.button.y - m->clip_rect.y) / m->tileSize;

            currentTile = m->getTile(gridX, gridY);

            switch (e.button.button) {
            case SDL_BUTTON_LEFT:
                if (currentTile == nullptr) {
                    selectedTile = nullptr;
                    break;
                }

                if (currentTile->state == STATE_FLOOR) {
                    mazeItem t = makeItem(ITEM_TELEPORT);
                    t.teleport.x = gridX;
                    t.teleport.y = gridY;
                    t.teleport.warpX = -1;
                    t.teleport.warpY = -1;
                    m->addItem(t);
                }

                item = m->findItem(currentTile);
                if (item != nullptr && item->type == ITEM_TELEPORT) {
                    state = EDIT_BUILDING;
                    selectedTile = currentTile;
                }

                break;
            case SDL_BUTTON_RIGHT:
                if (selectedTile == nullptr) break;

                item = m->findItem(selectedTile);
                if (item == nullptr) break;

                if (currentTile != nullptr && currentTile->state != STATE_BLOCK) {
                    if (currentTile == selectedTile) {
                        item->teleport.warpX = -1;
                        item->teleport.warpY = -1;
                    } else {
                        item->teleport.warpX = gridX;
                        item->teleport.warpY = gridY;
                    }
                }
                state = EDIT_MOVING_WARP;
                break;
            case SDL_BUTTON_MIDDLE:
                if (selectedTile != nullptr)
                    state = EDIT_TPCHAR;
                break;
            }
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (state != EDIT_TPCHAR)
            state = EDIT_IDLE;
        break;
    case SDL_MOUSEMOTION:
        if (currentTile == nullptr) break;

        gridX = (e.motion.x - m->clip_rect.x) / m->tileSize;
        gridY = (e.motion.y - m->clip_rect.y) / m->tileSize;
        newTile = m->getTile(gridX, gridY);

        if (newTile == nullptr) break;
        if (newTile == currentTile) break;

        switch (state) {
        case EDIT_BUILDING:
            item = m->findItem(currentTile);

            if (item != nullptr && newTile->state == STATE_FLOOR) {
                m->moveItem(*item, gridX, gridY);

                selectedTile = newTile;
                currentTile = newTile;
            }

            break;
        case EDIT_MOVING_WARP:
            if (selectedTile == nullptr) break;
            item = m->findItem(selectedTile);

            if (item != nullptr && newTile != nullptr && newTile->state != STATE_BLOCK) {
                if (newTile == selectedTile) {
                    item->teleport.warpX = -1;
                    item->teleport.warpY = -1;
                } else {
                    item->teleport.warpX = gridX;
                    item->teleport.warpY = gridY;
                }
            }

            currentTile = newTile;
            break;
        }
        break;
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_DELETE:
        case SDL_SCANCODE_BACKSPACE:
            item = m->findItem(selectedTile);
            if (item == nullptr) break;
            m->removeItem(*item);
            selectedTile = nullptr;
            state = EDIT_IDLE;
            break;
        default:
            break;
        }
        break;
    }
}

void toolTeleport::resetState() {
    state = EDIT_IDLE;
    selectedTile = nullptr;
}

void toolTeleport::render(SDL_Renderer *renderer) {
    renderSelection(renderer, selectedTile);

    if (selectedTile == nullptr) return;

    mazeItem *t = m->findItem(selectedTile);
    if (t == nullptr) return;

    if (t->teleport.warpX>=0 && t->teleport.warpY>=0) {
        SDL_Rect srcrect = {
            (timer / FRAME_TIME + 1) * 2 * RES_TILES_SIZE, 3 * RES_TILES_SIZE,
            2 * RES_TILES_SIZE, 2 * RES_TILES_SIZE
        };
        SDL_Rect dstrect = {
            t->teleport.warpX * m->tileSize + m->clip_rect.x - 4 * m->tileSize / RES_TILES_SIZE,
            t->teleport.warpY * m->tileSize + m->clip_rect.y - 4 * m->tileSize / RES_TILES_SIZE,
            m->tileSize * 2, m->tileSize * 2
        };
        SDL_RenderCopy(renderer, RES_TILES, &srcrect, &dstrect);
    }

    if (state == EDIT_TPCHAR) {
        SDL_Rect srcrect = {0, 0, 0, 0};
        SDL_QueryTexture(RES_TP_FONT, NULL, NULL, &srcrect.w, &srcrect.h);
        SDL_Rect &dstrect = tpCharDialRect;
        dstrect = {
            t->teleport.x * m->tileSize + m->clip_rect.x,
            t->teleport.y * m->tileSize + m->clip_rect.y,
            srcrect.w, srcrect.h
        };
        dstrect.x -= (t->teleport.tpChar % RES_TP_FONT_NUM) * RES_TP_FONT_SIZE;
        dstrect.y -= (t->teleport.tpChar / RES_TP_FONT_NUM) * RES_TP_FONT_SIZE;
        if (dstrect.x + dstrect.w > windowWidth) {
            dstrect.x = windowWidth - dstrect.w;
        }
        if (dstrect.x < 0) {
            dstrect.x = 0;
        }
        if (dstrect.y + dstrect.h > windowHeight) {
            dstrect.y = windowHeight - dstrect.h;
        }
        if (dstrect.y < 0) {
            dstrect.y = 0;
        }
        SDL_RenderCopy(renderer, RES_TP_FONT, &srcrect, &dstrect);
    }
}

void toolTeleport::tick() {
    if (++timer >= FRAME_TIME * 3)
        timer = 0;
}
