#include "maze_io.h"

#include "bit_array.h"

#include <fstream>

static inline void writeToBuffer(ofstream &ofs, const char *data, int length) {
	ofs.write(data, length);
}

static void writeInt(ofstream &ofs, const int num) {
	char str[4];
	str[0] = (num & 0xff000000) >> (8 * 3);
	str[1] = (num & 0x00ff0000) >> (8 * 2);
	str[2] = (num & 0x0000ff00) >> (8 * 1);
	str[3] = (num & 0x000000ff) >> (8 * 0);

	writeToBuffer(ofs, str, 4);
}

static void writeShort(ofstream &ofs, const short num) {
	char str[2];
	str[0] = (num & 0xff00) >> (8 * 1);
	str[1] = (num & 0x00ff) >> (8 * 0);

	writeToBuffer(ofs, str, 2);
}

static inline void writeChar(ofstream &ofs, const char num) {
	writeToBuffer(ofs, &num, 1);
}

static void writeString(ofstream &ofs, const string &str) {
	int len = (int)str.size();
	if (len <= 0) {
		writeInt(ofs, 0xffffffff);
	} else {
		writeInt(ofs, len);
		writeToBuffer(ofs, str.c_str(), len);
	}
}

static void writeWalls(ofstream &ofs, maze *m) {
	int x = 0;
	int y = 0;
	int w = m->width();
	int h = m->height();

	bitArray bits(w * (h + 1) + (w + 1) * h);
	int i = 0;

	for (y = 0; y <= h; ++y) {
		for (x = 0; x < w; ++x) {
			bits[i] = m->hwalls[y][x];
			++i;
		}
	}

	for (x = 0; x <= w; ++x) {
		for (y = 0; y < h; ++y) {
			bits[i] = m->vwalls[x][y];
			++i;
		}
	}

	writeToBuffer(ofs, "WALL", 4);
	writeInt(ofs, bits.length());
	
	bits.write(ofs);
}

void writeBlocks(ofstream &ofs, maze *m) {
	for (int i = 0; i < m->datasize(); ++i) {
		tile *t = m->getTile(i);
		if (t->state == STATE_BLOCK) {
			writeToBuffer(ofs, "BLOC", 4);
			writeInt(ofs, i);
		}
	}
}

void writeStartend(ofstream &ofs, maze *m) {
	tile *startTile = m->startTile();
	if (startTile != nullptr) {
		writeToBuffer(ofs, "STRT", 4);
		writeInt(ofs, m->getIndex(startTile));
	}

	tile *endTile = m->endTile();
	if (endTile != nullptr) {
		writeToBuffer(ofs, "ENDD", 4);
		writeInt(ofs, m->getIndex(endTile));
	}
}

void writeItems(ofstream &ofs, maze *m) {
	int w = m->width();

	for (pair<const int, mazeItem> &p : m->items) {
		const mazeItem &item = p.second;
		switch (item.type) {
		case ITEM_TELEPORT:
			writeToBuffer(ofs, "TELE", 4);
			writeChar(ofs, item.teleport.tpChar);
			if (item.teleport.warpX < 0 || item.teleport.warpY < 0) {
				writeInt(ofs, 0xffffffff);
			} else {
				writeInt(ofs, item.teleport.warpY * w + item.teleport.warpX);
			}
			writeInt(ofs, item.teleport.y * w + item.teleport.x);
			break;
		case ITEM_BRIDGE:
			writeToBuffer(ofs, "BRDG", 4);
			writeInt(ofs, item.bridge.y * w + item.bridge.x);
			break;
		case ITEM_ARROW:
			writeToBuffer(ofs, "ARRW", 4);
			writeInt(ofs, (item.arrow.y * w + item.arrow.x) * 4 + item.arrow.direction);
			break;
		}
	}
}

void saveMaze(const char *filename, maze *m) {
	ofstream ofs(filename, ofstream::out | ofstream::binary);

	writeInt(ofs, MAGIC_NUMBER);
	writeInt(ofs, VERSION_NUMBER);

	writeShort(ofs, m->width());
	writeShort(ofs, m->height());

	if (m->tileSize != DEFAULT_TILE_SIZE) {
		writeToBuffer(ofs, "TILE", 4);
		writeInt(ofs, m->tileSize);
	}

	if (! m->name.empty()) {
		writeToBuffer(ofs, "NAME", 4);
		writeString(ofs, m->name);
	}

	writeWalls(ofs, m);
	writeBlocks(ofs, m);
	writeStartend(ofs, m);
	writeItems(ofs, m);

	ofs.flush();
}
