#include "maze_editor.h"

#include "resources.h"

#include "tool_select.h"
#include "tool_crop.h"
#include "tool_wall.h"
#include "tool_bridge.h"
#include "tool_startend.h"
#include "tool_block.h"
#include "tool_arrow.h"
#include "tool_teleport.h"

extern SDL_Window *window;

mazeEditor::mazeEditor() {
    m = nullptr;

    showToolbox = false;
    currentTool = 0;

    toolboxTimer = TOOLBOX_TIMER;

	tools[0] = std::make_unique<toolSelect>(1);
	tools[1] = std::make_unique<toolCrop>(9);
	tools[2] = std::make_unique<toolWall>(3);
	tools[3] = std::make_unique<toolBridge>(4);
	tools[4] = std::make_unique<toolStartend>(5);
	tools[5] = std::make_unique<toolBlock>(6);
	tools[6] = std::make_unique<toolArrow>(7);
	tools[7] = std::make_unique<toolTeleport>(8);
}

void mazeEditor::setMaze(maze *_m) {
    m = _m;
    for (auto &tool : tools) {
        tool->setMaze(m);
        tool->resetState();
    }
}

void mazeEditor::handleEvent(SDL_Event &e) {
    if (m == nullptr) return;

    bool ctrlDown = (SDL_GetModState() & KMOD_CTRL) != 0;

	bool sendHandle = true;

    switch (e.type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if (e.button.windowID != SDL_GetWindowID(window)) {
			sendHandle = false;
		}
		break;
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_T:
            showToolbox = !showToolbox;
            toolboxTimer = showToolbox;
            break;
        default:
            if (e.key.keysym.scancode >= SDL_SCANCODE_1 &&
                e.key.keysym.scancode < SDL_SCANCODE_1 + (int)tools.size() && e.key.keysym.scancode <= SDL_SCANCODE_0) {
                tools[currentTool]->resetState();
                currentTool = e.key.keysym.scancode - SDL_SCANCODE_1;
                toolboxTimer = TOOLBOX_TIMER;
            }
            break;
        }
        break;
    case SDL_MOUSEMOTION:
        if (showToolbox) {
            if (inToolbox(e.motion.x, e.motion.y)) {
                toolboxTimer = 0;
            } else {
                toolboxTimer = 1;
            }
        }
        break;
    case SDL_MOUSEWHEEL:
        if (ctrlDown) {
            m->tileSize += e.wheel.y;
            if (m->tileSize < 3) {
                m->tileSize = 3;
            }
        } else {
            tools[currentTool]->resetState();
            if (e.wheel.y < 0) {
                toolboxTimer = TOOLBOX_TIMER;

                ++currentTool;
                if (currentTool >= tools.size()) {
                    currentTool = 0;
                }
            } else if (e.wheel.y > 0) {
                toolboxTimer = TOOLBOX_TIMER;

                if (currentTool == 0) {
                    currentTool = tools.size()-1;
                } else {
                    --currentTool;
                }
            }
        }
        break;
    }

    if (!ctrlDown && sendHandle) {
        tools[currentTool]->handleEvent(e);
    }
}

void mazeEditor::render(SDL_Renderer *renderer, int ox, int oy) {
    tools[currentTool]->render(renderer);

    if (toolboxTimer > 0) {
        if (!showToolbox)
            --toolboxTimer;

        SDL_Rect srcrect = {0, 0, RES_TOOLS_SIZE, RES_TOOLS_SIZE};
        SDL_Rect dstrect = {ox, oy, RES_TOOLS_SIZE, RES_TOOLS_SIZE};

        SDL_Rect select_srcrect = {0, 0, RES_TOOLS_SIZE, RES_TOOLS_SIZE};
        SDL_Rect select_dstrect = {ox, (int)currentTool * RES_TOOLS_SIZE + oy, RES_TOOLS_SIZE, RES_TOOLS_SIZE};

        SDL_RenderCopy(renderer, RES_TOOLS, &select_srcrect, &select_dstrect);

        for (size_t i=0; i<tools.size(); ++i) {
            const Uint8 sn = tools[i]->spriteNum();
            srcrect.x = (sn * RES_TOOLS_SIZE) % (RES_TOOLS_SIZE * RES_TOOLS_NUM);
            srcrect.y = (sn * RES_TOOLS_SIZE) / (RES_TOOLS_SIZE * RES_TOOLS_NUM) * RES_TOOLS_SIZE;
            dstrect.y = i * RES_TOOLS_SIZE + oy;
            SDL_RenderCopy(renderer, RES_TOOLS, &srcrect, &dstrect);
        }

        clip_rect.x = ox;
        clip_rect.y = oy;
        clip_rect.w = RES_TOOLS_SIZE;
        clip_rect.h = RES_TOOLS_SIZE * tools.size();
    }
}

void mazeEditor::tick() {
    tools[currentTool]->tick();
}

bool mazeEditor::inToolbox(int x, int y) {
    return
        x >= clip_rect.x && x <= clip_rect.x + clip_rect.w &&
        y >= clip_rect.y && y <= clip_rect.y + clip_rect.h;
}
