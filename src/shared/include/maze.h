#ifndef __MAZE_H__
#define __MAZE_H__

#include "tile.h"
#include "grid.h"
#include "wall.h"

#include "maze_items.h"

#include <vector>
#include <map>
#include <string>



extern const int DEFAULT_TILE_SIZE;

struct rect {
    int x;
    int y;
    int w;
    int h;
};

class maze: public grid<tile> {
public:
    std::vector<wall> hwalls;
    std::vector<wall> vwalls;

    std::map<int, mazeItem> items;

    int version;
    std::string name;
    bool renderGrid;

    int tileSize;

    rect clip_rect;

public:
    maze(int w, int h);

public:
    void clearTemp();

    void resizeMaze(int newW, int newH);

    void moveMaze(int ox, int oy);

public:
    tile *startTile();
    tile *endTile();

    tile *getItem(int index) const = delete;
    tile *getItem(int x, int y) const = delete;

    tile *getTile(int index) const {
        return grid::getItem(index);
    }
    tile *getTile(short x, short y) const {
        return grid::getItem(x, y);
    }

    void addItem(const mazeItem &item);
    void removeItem(const mazeItem &item);
    void moveItem(mazeItem &item, short x, short y);

    mazeItem *findItem(tile *parent);
    mazeItem *findItem(short x, short y);
    mazeItem *findItem(int index);

    tile *getItemTile(const mazeItem &item);

private:
    void updateMoveItem(mazeItem &m, short x, short y);

    void moveWalls(int ox, int oy);
};
#endif // __MAZE_H__
