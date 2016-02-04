#include "maze.h"

#include <cstring>
#include <algorithm>

const int DEFAULT_TILE_SIZE = 10;

maze::maze(int w, int h) : grid<tile>(w, h) {
    name = "";
    renderGrid = false;

    memset(data, 0, datasize() * sizeof *data);

    hwalls.resize(h+1, wall(w));
    vwalls.resize(w+1, wall(h));

    tileSize = DEFAULT_TILE_SIZE;
}

void maze::addItem(const mazeItem &item) {
    getTile(item.item.x, item.item.y)->state = STATE_ITEM;
    items.insert(pair<int, const mazeItem &>(itemIndex(item, w), item));
}

void maze::removeItem(const mazeItem &item) {
    getTile(item.item.x, item.item.y)->state = STATE_FLOOR;
    items.erase(itemIndex(item, w));
}

void maze::updateMoveItem(mazeItem &item, short x, short y) {
    switch (item.type) {
    case ITEM_BRIDGE:
        item.bridge.upperTile.temp = TEMP_NULL;
        break;
    case ITEM_TELEPORT:
        for (pair<const int, mazeItem> &it : items) {
            if (it.second.type == ITEM_TELEPORT) {
                if (it.second.teleport.warpX == item.teleport.x && it.second.teleport.warpY == item.teleport.y) {
                    it.second.teleport.warpX = x;
                    it.second.teleport.warpY = y;
                }
            }
        }
        break;
    }

    item.item.x = x;
    item.item.y = y;
}

void maze::moveItem(mazeItem &item, short x, short y) {
    if (x == item.item.x && y == item.item.y) return;

    int oldIndex = itemIndex(item, w);

    updateMoveItem(item, x, y);

    int newIndex = itemIndex(item, w);

    getTile(oldIndex)->state = STATE_FLOOR;
    getTile(newIndex)->state = STATE_ITEM;

    items[newIndex] = items[oldIndex];
    items.erase(oldIndex);
}

mazeItem *maze::findItem(tile *parent) {
    if (parent->state == STATE_ITEM) {
        return findItem(getIndex(parent));
    }
    return nullptr;
}

mazeItem *maze::findItem(short x, short y) {
    if (x >= 0 && x < w && y >= 0 && y < h) {
        return findItem(y * w + x);
    }
    return nullptr;
}

mazeItem *maze::findItem(int index) {
    auto it = items.find(index);
    if (it != items.end()) {
        return &(it->second);
    }
    return nullptr;
}

tile *maze::getItemTile(const mazeItem &item) {
    return getTile(item.item.x, item.item.y);
}

tile *maze::startTile() {
    for (tile *t = data; t-data<datasize(); ++t) {
        if (t->state == STATE_START)
            return t;
    }
    return nullptr;
}

tile *maze::endTile() {
    for (tile *t = data; t-data<datasize(); ++t) {
        if (t->state == STATE_END)
            return t;
    }
    return nullptr;
}

void maze::clearTemp() {
    for (tile *t = data; t-data<datasize(); ++t) {
        t->temp = TEMP_NULL;
    }

    for (pair<const int, mazeItem> &it : items) {
        if (it.second.type == ITEM_BRIDGE) {
            it.second.bridge.upperTile.temp = TEMP_NULL;
        }
    }
}

void maze::resizeMaze(int newW, int newH) {
    if (data == nullptr) return;

    hwalls.resize(newH + 1);
    for (wall &i : hwalls) {
        i.resizeWall(newW);
    }
    vwalls.resize(newW + 1);
    for (wall &i : vwalls) {
        i.resizeWall(newH);
    }

    maze newMaze(newW, newH);
    newMaze.copyGrid(*this);

    w = newW;
    h = newH;

    delete[]data;
    data = new tile[w*h];

    memcpy(data, newMaze.data, w*h * sizeof *data);

    map<int, mazeItem> newMap;
    for (pair<const int, mazeItem> &it : items) {
        mazeItem &item = it.second;
        if (item.item.x < w && item.item.y < h) {
            newMap.insert(pair<int, const mazeItem &>(itemIndex(item, w), item));
        }
    }
    items.clear();
    items.insert(newMap.begin(), newMap.end());
}

void maze::moveWalls(int ox, int oy) {
    if (ox != 0) {
        for (wall &i : hwalls) {
            i.moveWall(ox);
        }
    }
    if (oy < 0) {
        hwalls.erase(hwalls.begin(), hwalls.begin()-oy);
        hwalls.insert(hwalls.end(), h+1-hwalls.size(), wall(w));
    } else if (oy > 0) {
        hwalls.erase(hwalls.end()-oy, hwalls.end());
        hwalls.insert(hwalls.begin(), h+1-hwalls.size(), wall(w));
    }
    if (oy != 0) {
        for (wall &i : vwalls) {
            i.moveWall(oy);
        }
    }
    if (ox < 0) {
        vwalls.erase(vwalls.begin(), vwalls.begin()-ox);
        vwalls.insert(vwalls.end(), w+1-vwalls.size(), wall(h));
    } else if (ox > 0) {
        vwalls.erase(vwalls.end()-ox, vwalls.end());
        vwalls.insert(vwalls.begin(), w+1-vwalls.size(), wall(h));
    }
}

void maze::moveMaze(int ox, int oy) {
    if (data == nullptr) return;
    if (ox == 0 && oy == 0) return;
	if (abs(ox) > w || abs(oy) > h) {
		grid::clear();
		hwalls.clear();
		vwalls.clear();
		items.clear();
		return;
	}

    tile emptyTile;
    memset(&emptyTile, 0, sizeof emptyTile);
    moveGrid(ox, oy, emptyTile);

    moveWalls(ox, oy);

    map<int, mazeItem> newMap;

    for (pair<const int, mazeItem> &it : items) {
        mazeItem &item = it.second;
        short x = item.item.x + ox;
        short y = item.item.y + oy;
        if (x >= 0 && x < w && y >= 0 && y < h) {
            item.item.x = x;
            item.item.y = y;
            newMap.insert(pair<int, const mazeItem &>(itemIndex(item, w), item));
        }
    }

    for (pair<const int, mazeItem> &it : newMap) {
        switch (it.second.type) {
        case ITEM_TELEPORT:
            it.second.teleport.warpX += ox;
            it.second.teleport.warpY += oy;
            break;
        }
    }
    items.clear();
    items.insert(newMap.begin(), newMap.end());
}
