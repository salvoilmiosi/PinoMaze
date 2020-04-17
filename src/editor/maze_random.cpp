#include "maze_random.h"

#include "maze_items.h"

#include <cstdlib>
#include <stack>
#include <vector>

struct cell {
    int x, y;
    int z = 0;
    bool visited = false;

    cell *neighbours[4];
    size_t numNeighbours = 0;
};

std::unique_ptr<maze> generateRandomMaze(int w, int h) {
	auto m = std::make_unique<maze>(w, h);

    m->hwalls.clear();
    m->hwalls.resize(h + 1, wall(w, 1));
    m->vwalls.clear();
    m->vwalls.resize(w + 1, wall(h, 1));

    cell cells[w * h * 2];

    auto getCell = [&](int x, int y, int z = 0) {
        if (x < 0 || x >= w) return (cell *) nullptr;
        if (y < 0 || y >= h) return (cell *) nullptr;
        return cells + z * w * h + y * w + x;
    };

    auto isBridge = [&](int x, int y) {
        tile *t = m->getTile(x, y);
        if (t && t->state == STATE_ITEM) {
            mazeItem *item = m->findItem(t);
            return item && item->type == ITEM_BRIDGE;
        }
        return false;
    };

    auto addNeighbour = [&](cell *c, cell *target) {
        if (target && c->numNeighbours < 4) {
            tile *t = m->getTile(target->x, target->y);
            if (t->state != STATE_BLOCK) {
                c->neighbours[c->numNeighbours] = target;
                ++c->numNeighbours;
            }
        }
    };

    auto addNeighbours = [&](cell *c) {
        if (c->z == 1) {
            if (isBridge(c->x, c->y)) {
                addNeighbour(c, getCell(c->x - 1, c->y, isBridge(c->x - 1, c->y))); // left
                addNeighbour(c, getCell(c->x + 1, c->y, isBridge(c->x + 1, c->y))); // right
            }
        } else {
            addNeighbour(c, getCell(c->x, c->y-1, 0)); // top
            addNeighbour(c, getCell(c->x, c->y+1, 0)); // bottom
            if (!isBridge(c->x, c->y)) {
                addNeighbour(c, getCell(c->x - 1, c->y, isBridge(c->x - 1, c->y))); // left
                addNeighbour(c, getCell(c->x + 1, c->y, isBridge(c->x + 1, c->y))); // right
            }
        }
        return c->numNeighbours;
    };

    auto checkNeighbours = [&](cell *c) {
        std::vector<cell *> neighbours;

        for (size_t i=0; i < c->numNeighbours; ++i) {
            if (!c->neighbours[i]->visited) {
                neighbours.push_back(c->neighbours[i]);
            }
        }

        return neighbours;
    };

    auto removeWall = [&](cell *a, cell *b) {
        if (a->x != b->x) {
            m->vwalls[std::max(a->x, b->x)][a->y] = 0;
        } else if (a->y != b->y) {
            m->hwalls[std::max(a->y, b->y)][a->x] = 0;
        }
    };

    auto removeWallsAround = [&](int x, int y) {
        m->hwalls[y][x] = 0;
        m->hwalls[y + 1][x] = 0;
        m->vwalls[x][y] = 0;
        m->vwalls[x + 1][y] = 0;
    };

    m->getTile(0, 0)->state = STATE_START;
    m->getTile(w - 1, h - 1)->state = STATE_END;

    size_t num_bridges = w * h / 10;
    for (size_t i=0; i < num_bridges; ++i) {
        mazeItem b = makeItem(ITEM_BRIDGE);
        b.bridge.wallValue = 1;
        do {
            b.bridge.x = rand() % w;
            b.bridge.y = rand() % h;
        } while (m->getTile(b.bridge.x, b.bridge.y)->state != STATE_FLOOR);
        m->addItem(b);
    }

    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            tile *t = m->getTile(x, y);
            if (rand() < RAND_MAX / 4 && t->state == STATE_FLOOR) {
                t->state = STATE_BLOCK;
                removeWallsAround(x, y);
            }
        }
    }

    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                cell *c = getCell(x, y, z);
                c->x = x;
                c->y = y;
                c->z = z;
            }
        }
    }

    for (size_t i=0; i < w * h * 2; ++i) {
        addNeighbours(cells + i);
    }

    std::stack<cell *> cellStack;
    cell *start = getCell(0, 0);
    start->visited = true;
    cellStack.push(start);
    while (!cellStack.empty()) {
        cell *current = cellStack.top();
        cellStack.pop();

        auto unvisited = checkNeighbours(current);
        if (!unvisited.empty()) {
            cellStack.push(current);
            cell *target = unvisited[rand() % unvisited.size()];
            removeWall(current, target);
            target->visited = true;
            cellStack.push(target);
        }
    }

    return m;
}
