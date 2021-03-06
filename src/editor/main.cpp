#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <memory>
#include <cstdio>

#include "a_star.h"

#include "maze_io.h"
#include "maze_random.h"
#include "maze_editor.h"
#include "maze_renderer.h"

#include "resources.h"

#include "gui.h"

using namespace std;

int windowWidth = 800;
int windowHeight = 600;
SDL_Window *window;

namespace {
	const float TICKRATE = 100.0;

	std::unique_ptr<a_star> mainGrid = nullptr;
	std::unique_ptr<maze> mainMaze = nullptr;
	std::unique_ptr<mazeEditor> editor = nullptr;

	int offsetx;
	int offsety;

	bool showFpsCounter = false;

	const int STATUS_TIMER = 300;
	const int STATUS_LENGTH = 128;

	std::string statusText;
	int statusTimer = STATUS_TIMER;

	std::string lastFilename = "";
}

static void setStatus(const char *status) {
	statusText = status;
	statusTimer = STATUS_TIMER;
	printf("%s\n", status);
}

static void resetMaze() {
	mainGrid->setMaze(mainMaze.get());
	editor->setMaze(mainMaze.get());

	offsetx = (windowWidth - mainMaze->width() * mainMaze->tileSize) / 2;
	offsety = (windowHeight - mainMaze->height() * mainMaze->tileSize) / 2;
}

static void setMaze(std::unique_ptr<maze> &&m) {
	if (!m) return;

	mainMaze = move(m);
	resetMaze();
}

static const char *fileExtension(const char *filename) {
    const char *index = nullptr;
    const char *lastDot = nullptr;
    for (index = filename; *index != '\0'; ++index) {
        if (*index == '.') lastDot = index;
    }
    if (lastDot == nullptr) return "";
    else return lastDot+1;
}

static bool openMazeFile() {
    const char *filename = getMazeFilename(DIALOG_OPEN);

    if (filename) {
		std::unique_ptr<maze> m = openMaze(filename);
        if (m == nullptr) {
            messageBox(MESSAGE_ERROR, "Cannot open this file");
            return false;
        } else {
			lastFilename = filename;
            setMaze(move(m));
            return true;
        }
    }
    return false;
}

static bool saveMazeFile() {
    const char *filename = getMazeFilename(DIALOG_SAVE, lastFilename.c_str());

    if (filename) {
        lastFilename = filename;
        if (strlen(fileExtension(filename)) == 0) {
            lastFilename += ".pmz";
        }
        saveMaze(filename, mainMaze.get());
        return true;
    }
    return false;
}

static void handleEvent(SDL_Event &e) {
    switch(e.type) {
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_R:
            setMaze(generateRandomMaze(mainMaze->width(), mainMaze->height()));
            setStatus("Random maze generated");
            break;
        case SDL_SCANCODE_O:
            if (openMazeFile()) {
                setStatus("Maze file opened");
				printf("%s\n", lastFilename.c_str());
            }
            break;
        case SDL_SCANCODE_A:
            switch (currentAlgorithm) {
            case ALGORITHM_ASTAR:
                currentAlgorithm = ALGORITHM_GBFS;
                setStatus("Selected algorithm: Greedy Best First Search");
                break;
            case ALGORITHM_GBFS:
                currentAlgorithm = ALGORITHM_DIJKSTRA;
                setStatus("Selected algorithm: Dijkstra");
                break;
            case ALGORITHM_DIJKSTRA:
                currentAlgorithm = ALGORITHM_ASTAR;
                setStatus("Selected algorithm: A*");
                break;
            }
            break;
        case SDL_SCANCODE_S:
            if (saveMazeFile()) {
                setStatus("Maze file saved");
				printf("%s\n", lastFilename.c_str());
            }
            break;
        case SDL_SCANCODE_F:
            showFpsCounter = !showFpsCounter;
            break;
        case SDL_SCANCODE_G:
			mainMaze->renderGrid = !mainMaze->renderGrid;
            break;
        case SDL_SCANCODE_C:
			mainMaze->clearTemp();
            mainGrid->stop();
            setStatus("Path finding stopped");
            break;
        case SDL_SCANCODE_N:
            setMaze(std::make_unique<maze>(windowWidth / RES_TILES_SIZE, windowHeight / RES_TILES_SIZE));
            setStatus("Created empty new maze");
            break;
        case SDL_SCANCODE_SPACE:
            mainGrid->start();
            setStatus("Path finding started");
            break;
        default:
            break;
        }
        break;
    case SDL_USEREVENT:
		resetMaze();
        setStatus("Maze has been cropped");
        break;
    }
    editor->handleEvent(e);
}

static void tick() {
    if (mainGrid != nullptr)
        mainGrid->tick();
    if (editor != nullptr)
        editor->tick();

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_LEFT])
        offsetx -= 3;
    if (keys[SDL_SCANCODE_RIGHT])
        offsetx += 3;
    if (keys[SDL_SCANCODE_UP])
        offsety -= 3;
    if (keys[SDL_SCANCODE_DOWN])
        offsety += 3;

    if (statusTimer > 0) {
        if (--statusTimer == 0) {
			statusText.clear();
        }
    }
}

static void renderText(SDL_Renderer *renderer, const char *str, int len, int ox, int oy) {
	SDL_Rect srcrect = { 0, 0, RES_TEXT_W, RES_TEXT_H };
	SDL_Rect dstrect = { ox, oy, srcrect.w, srcrect.h };
	for (const char *c = str; *c != '\0' && c - str<len; ++c) {
		if (*c != '\n' && *c != '\r') {
			srcrect.x = (*c % RES_TEXT_NUM) * srcrect.w;
			srcrect.y = (*c / RES_TEXT_NUM) * srcrect.h;
			SDL_RenderCopy(renderer, RES_TEXT, &srcrect, &dstrect);
		}

		if (*c == '\n') {
			dstrect.x = ox;
			dstrect.y += dstrect.h;
		} else {
			dstrect.x += dstrect.w;
		}
	}
}

static void render(SDL_Renderer *renderer, char *fpsCounter) {
    SDL_SetRenderDrawColor(renderer, COLOR_BG);
    SDL_RenderClear(renderer);

    renderMaze(mainMaze.get(), renderer, offsetx, offsety);
    editor->render(renderer, 5, 5);

    if (showFpsCounter) {
        renderText(renderer, fpsCounter, STATUS_LENGTH, 5, 5);
    }
    if (statusTimer > 0) {
        renderText(renderer, statusText.c_str(), STATUS_LENGTH, 5, windowHeight - 19);
    }

    SDL_RenderPresent(renderer);
}

int main (int argc, char **argv) {
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		return -1;

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		return -1;

    srand(SDL_GetTicks());

    window = SDL_CreateWindow("PinoMaze level editor",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
        return 2;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    std::string res_path = argv[0];
    res_path = res_path.substr(0, 1 + res_path.find_last_of("\\/"));
    res_path += "editor.dat";

	if (!openResourceFile(res_path.c_str()) || !loadResources(renderer)) {
		fprintf(stderr, "Could not load resources\n");
		return 3;
	}

	editor = std::make_unique<mazeEditor>();
	mainGrid = std::make_unique<a_star>();

	if (argc > 1) {
		setMaze(openMaze(argv[1]));
	}

	if (!mainMaze) {
		setMaze(std::make_unique<maze>(windowWidth / RES_TILES_SIZE, windowHeight / RES_TILES_SIZE));
	}

    setStatus("Welcome to PinoMaze level editor");

    SDL_Event event;

    bool quit = false;

    int lastTime = SDL_GetTicks();
    float unprocessed = 0;
    float msPerTick = 1000.0 / TICKRATE;
    int frames = 0;
    int ticks = 0;
    int secondsTimer = SDL_GetTicks();

    char fpsCounter[STATUS_LENGTH];
    memset(fpsCounter, 0, STATUS_LENGTH);

    while(!quit) {
        int now = SDL_GetTicks();
        unprocessed += (now - lastTime) / msPerTick;
        lastTime = now;
        bool shouldRender = true;
        while (unprocessed >= 1) {
            tick();

            ++ticks;
            --unprocessed;
            shouldRender = true;
        }

        SDL_Delay(2);

        if (shouldRender) {
            render(renderer, fpsCounter);
            ++frames;
        }

        while(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_RESIZED:
                    windowWidth = event.window.data1;
                    windowHeight = event.window.data2;
                    break;
                }
                break;
            case SDL_QUIT:
                quit = true;
                break;
            default:
                handleEvent(event);
            }
        }

        if (SDL_GetTicks() - secondsTimer > 1000) {
            secondsTimer += 1000;
            snprintf(fpsCounter, STATUS_LENGTH, "Frames: %d\nTicks: %d", frames, ticks);
            frames = 0;
            ticks = 0;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
