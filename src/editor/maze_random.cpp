#include "maze_random.h"

#include "maze_items.h"
#include "grid3d.h"

#include <cstdlib>
#include <stack>

struct cell {
    bool visited = false;
    cell *neighbours[4];
    size_t num_neighbours = 0;
};

std::unique_ptr<maze> generateRandomMaze(int w, int h) {
	auto m = std::make_unique<maze>(w, h);

    m->hwalls.clear();
    m->hwalls.resize(h + 1, wall(w, 1));
    m->vwalls.clear();
    m->vwalls.resize(w + 1, wall(h, 1));

    grid3d<cell> cells(w, h, 2);

    auto getCell = [&](int x, int y, int z = 0) {
        return cells.getItem(x, y, z);
    };

    auto isBridge = [&](int x, int y) {
        tile *t = m->getTile(x, y);
        if (t && t->state == STATE_ITEM) {
            mazeItem *item = m->findItem(t);
            return item && item->type == ITEM_BRIDGE;
        }
        return false;
    };

    auto addNeighbour = [&](cell *c, int x, int y, int z) {
        cell *target = getCell(x, y, z);
        if (target) {
            tile *t = m->getTile(x, y);
            if (t->state != STATE_BLOCK && c->num_neighbours < 4) {
                c->neighbours[c->num_neighbours++] = target;
            }
        }
    };

    auto addNeighbours = [&](int x, int y, int z) {
        cell *c = getCell(x, y, z);
        if (z == 1) {
            if (isBridge(x, y)) {
                addNeighbour(c, x - 1, y, isBridge(x - 1, y)); // left
                addNeighbour(c, x + 1, y, isBridge(x + 1, y)); // right
            }
        } else {
            addNeighbour(c, x, y - 1, 0); // top
            addNeighbour(c, x, y + 1, 0); // bottom
            if (!isBridge(x, y)) {
                addNeighbour(c, x - 1, y, isBridge(x - 1, y)); // left
                addNeighbour(c, x + 1, y, isBridge(x + 1, y)); // right
            }
        }
    };

    auto checkUnvisited = [&](cell **unvisited, cell *c) {
        size_t num = 0;

        for (size_t i=0; i<c->num_neighbours; ++i) {
            if (!c->neighbours[i]->visited) {
                unvisited[num] = c->neighbours[i];
                ++num;
            }
        }

        return num;
    };

    auto removeWall = [&](cell *ca, cell *cb) {
        auto a = cells.getCoords(ca);
        auto b = cells.getCoords(cb);
        if (a.x != b.x) {
            m->vwalls[std::max(a.x, b.x)][a.y] = 0;
        } else if (a.y != b.y) {
            m->hwalls[std::max(a.y, b.y)][a.x] = 0;
        }
    };

    auto removeWallsAround = [&](int x, int y) {
        m->hwalls[y][x] = 0;
        m->hwalls[y + 1][x] = 0;
        m->vwalls[x][y] = 0;
        m->vwalls[x + 1][y] = 0;
    };

    cell *start = getCell(rand() % w, rand() % h);
    auto start_pos = cells.getCoords(start);
    m->getTile(start_pos.x, start_pos.y)->state = STATE_START;

    size_t num_bridges = w * h / 10;
    for (size_t i=0; i < num_bridges; ++i) {
        mazeItem b = makeItem(ITEM_BRIDGE);
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
            }
        }
    }

    for (int z = 0; z < 2; ++z) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                addNeighbours(x, y, z);
            }
        }
    }

    std::stack<cell *> cellStack;

    cell *furthest = start;
    size_t top_size = 0;
    
    start->visited = true;
    cellStack.push(start);
    while (!cellStack.empty()) {
        cell *current = cellStack.top();

        cell *unvisited[4];
        size_t num_unvisited = checkUnvisited(unvisited, current);
        if (num_unvisited > 0) {
            cell *target = unvisited[rand() % num_unvisited];
            removeWall(current, target);
            target->visited = true;
            cellStack.push(target);
            if (cellStack.size() > top_size) {
                auto pos = cells.getCoords(target);
                tile *t = m->getTile(pos.x, pos.y);
                if (t && t->state == STATE_FLOOR) {
                    top_size = cellStack.size();
                    furthest = target;
                }
            }
        } else {
            cellStack.pop();
        }
    }

    auto exit_pos = cells.getCoords(furthest);
    m->getTile(exit_pos.x, exit_pos.y)->state = STATE_END;

    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            cell *c = getCell(x, y);
            if (! c->visited) {
                tile *t = m->getTile(x, y);
                if (t && t->state == STATE_ITEM) {
                    m->removeItem(*m->findItem(t));
                }
                removeWallsAround(x, y);
            }
        }
    }

    return m;
}
