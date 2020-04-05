#ifndef __TILE_H__
#define __TILE_H__

enum tileState {
	STATE_FLOOR,
	STATE_BLOCK,
	STATE_START,
	STATE_END,
	STATE_ITEM,
};

enum tileTemp {
	TEMP_NULL,
	TEMP_NODE_CLOSED,
	TEMP_NODE_OPEN,
	TEMP_PATH,
	TEMP_PATH_OLD,
	TEMP_START,
	TEMP_END,
};

struct tile {
	tileState state;
	tileTemp temp;
};

#endif // __TILE_H__
