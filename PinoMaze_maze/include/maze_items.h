#ifndef __MAZE_ITEMS_H__
#define __MAZE_ITEMS_H__

#include "tile.h"

enum itemType {
	ITEM_GENERIC,
	ITEM_TELEPORT,
	ITEM_BRIDGE,
	ITEM_ARROW,
};

enum arrowDirection {
	ARROW_UP = 0,
	ARROW_LEFT = 1,
	ARROW_DOWN = 2,
	ARROW_RIGHT = 3,
};

struct _generic {
    itemType type;

    short x;
    short y;
};

struct _teleport {
    itemType type;

    short x;
    short y;

    short warpX;
    short warpY;
    unsigned char tpChar;
};

struct _bridge {
    itemType type;

    short x;
    short y;

    tile upperTile;
};

struct _arrow {
    itemType type;

    short x;
    short y;

    arrowDirection direction;
};

union mazeItem {
	itemType type;

    _generic item;
    _teleport teleport;
    _bridge bridge;
    _arrow arrow;
};

int itemIndex(const mazeItem &item, int w);

mazeItem makeItem(itemType type = ITEM_GENERIC);

bool arrowPointsTo(const mazeItem &item, int ox, int oy);

#endif // __MAZE_ITEM_H__
