#include "tool_crop.h"

#include "resources.h"

void toolCrop::handleEvent(SDL_Event &e) {
    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        startX = e.button.x - m->clip_rect.x;
        if (startX < 0) startX -= m->tileSize;
        startX /= m->tileSize;

        startY = e.button.y - m->clip_rect.y;
        if (startY < 0) startY -= m->tileSize;
        startY /= m->tileSize;

        endX = startX;
        endY = startY;

        state = EDIT_BUILDING;
        break;
    case SDL_MOUSEBUTTONUP:
        state = EDIT_CROP;
        break;
    case SDL_MOUSEMOTION:
        if (state == EDIT_BUILDING) {
            endX = e.button.x - m->clip_rect.x;
            if (endX < 0) endX -= m->tileSize;
            endX /= m->tileSize;

            endY = e.button.y - m->clip_rect.y;
            if (endY < 0) endY -= m->tileSize;
            endY /= m->tileSize;
        }
        break;
    case SDL_KEYDOWN:
        if (e.key.keysym.scancode == SDL_SCANCODE_RETURN && state == EDIT_CROP) {
            if (startX > endX) {
                int tmp = startX;
                startX = endX;
                endX = tmp;
            }
            if (startY > endY) {
                int tmp = startY;
                startY = endY;
                endY = tmp;
            }
            int newW = 1 + endX - startX;
            int newH = 1 + endY - startY;
            if (newW > m->width() || newH > m->height()) {
                m->resizeMaze(newW, newH);
                m->moveMaze(-startX, -startY);
            } else {
                m->moveMaze(-startX, -startY);
                m->resizeMaze(newW, newH);
            }
            SDL_Event updateEvent;
            SDL_zero(updateEvent);
            updateEvent.user.type = SDL_USEREVENT;
            SDL_PushEvent(&updateEvent);
            state = EDIT_IDLE;
        }
        break;
    }
}

void toolCrop::render(SDL_Renderer *renderer) {
    if (state == EDIT_IDLE) return;

    int x0,y0,x1,y1;
    if (startX < endX) {
        x0 = startX;
        x1 = endX + 1;
    } else {
        x0 = endX;
        x1 = startX + 1;
    }
    if (startY < endY) {
        y0 = startY;
        y1 = endY + 1;
    } else {
        y0 = endY;
        y1 = startY + 1;
    }

    SDL_Rect rect = {
        x0 * m->tileSize + m->clip_rect.x,
        y0 * m->tileSize + m->clip_rect.y,
        (x1-x0) * m->tileSize,
        (y1-y0) * m->tileSize
    };

    SDL_Color color = COLOR_CROP_RECT;
    if (state == EDIT_CROP && timer > BLINK_TIMER) color = COLOR_CROP_BLINK;
    SDL_SetRenderDrawColor(renderer, color);
    SDL_RenderDrawRect(renderer, &rect);
}

void toolCrop::tick() {
    ++timer;
    if (timer > BLINK_TIMER * 2)
        timer = 0;
}
