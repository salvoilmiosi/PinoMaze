#include "maze_io.h"

#include "bit_array.h"

#include <fstream>
#include <string>

static inline void readData(char *out, std::ifstream &ifs, int length) {
	ifs.read(out, length);
}

static inline unsigned char readChar(std::ifstream &ifs) {
	return ifs.get();
}

static unsigned int readInt(std::ifstream &ifs) {
	char data[4];
	readData(data, ifs, 4);
    unsigned int num =
        (data[0] << 24 & 0xff000000) |
        (data[1] << 16 & 0x00ff0000) |
        (data[2] <<  8 & 0x0000ff00) |
        (data[3]       & 0x000000ff);
    return num;
}

static std::string readString(std::ifstream &ifs) {
    unsigned int length = readInt(ifs);
	std::string out;
    if (length != 0xffffffff) {
		char *data = new char[length];
		readData(data, ifs, length);
		out = data;
		delete[]data;
    }
    return out;
}

static unsigned short int readShort(std::ifstream &ifs) {
	char data[2];
	readData(data, ifs, 2);
    unsigned short int num =
        (data[0] << 8 & 0xff00) |
        (data[1]      & 0x00ff);
    return num;
}

static void readWalls(maze *m, std::ifstream &ifs) {
    bitArray bits(readInt(ifs));
    bits.read(ifs);

    int x = 0;
    int y = 0;
    int w = m->width();
    int h = m->height();
    int i = 0;

    switch (m->version) {
    case 0x01000000:
        for (y = 0; y <= h; ++y) {
            for (x = 0; x < w; ++x) {
                m->hwalls[y][x] = bits[i];
                ++i;
            }
        }

        for (x = 0; x <= w; ++x) {
            for (y = 0; y < h; ++y) {
                m->vwalls[x][y] = bits[i];
                ++i;
            }
        }
        break;
    case 0x02000000:
        for (y = 0; y <= h; ++y) {
            for (x = 0; x < w; ++x) {
                m->hwalls[y][x] = ((int) bits[i] << 1) | (int) bits[i+1];
                i += 2;
            }
        }

        for (x = 0; x <= w; ++x) {
            for (y = 0; y < h; ++y) {
                m->vwalls[x][y] = ((int) bits[i] << 1) | (int) bits[i+1];
                i += 2;
            }
        }
        break;
    }
}

static void readBlocks(maze *m, std::ifstream &ifs) {
    switch (m->version) {
    case 0x01000000:
        if (tile *t = m->getTile(readInt(ifs))) {
            t->state = STATE_BLOCK;
        }
        break;
    case 0x02000000:
    {
        bitArray bits(readInt(ifs));
        bits.read(ifs);
        for (int i=0; i<bits.length(); ++i) {
            if (bits[i]) {
                tile *t = m->getTile(i);
                t->state = STATE_BLOCK;
            }
        }
        break;
    }
    default:
        break;
    }
}

static constexpr unsigned int str2int(const char *str) {
    return
        str[0] << (8 * 3) |
        str[1] << (8 * 2) |
        str[2] << (8 * 1) |
        str[3] << (8 * 0);
}

static bool readFrame(maze *m, std::ifstream &ifs) {
    int i = 0;
    int w = m->width();

    switch (readInt(ifs)) {
	case str2int("TILE"):
        m->tileSize = readInt(ifs);
        break;
	case str2int("NAME"):
		m->name = readString(ifs);
		break;
	case str2int("WALL"):
        readWalls(m, ifs);
        break;
	case str2int("STRT"):
        i = readInt(ifs);
        if (!m->startTile()) {
            m->getTile(i%w, i/w)->state = STATE_START;
        }
        break;
	case str2int("ENDD"):
        i = readInt(ifs);
        if (!m->endTile()) {
            m->getTile(i%w, i/w)->state = STATE_END;
        }
        break;
	case str2int("BRDG"):
    {
        i = readInt(ifs);
        mazeItem b = makeItem(ITEM_BRIDGE);
        b.bridge.x = i%w;
        b.bridge.y = i/w;
        m->addItem(b);
        break;
    }
	case str2int("ARRW"):
    {
        i = readInt(ifs);
        mazeItem a = makeItem(ITEM_ARROW);
        a.arrow.x = (i/4) % w;
        a.arrow.y = (i/4) / w;
        a.arrow.direction = (arrowDirection) (i % 4);
        m->addItem(a);
        break;
    }
	case str2int("BLOC"):
        readBlocks(m, ifs);
        break;
	case str2int("TELE"):
    {
        unsigned char tpChar = readChar(ifs);
        int warp = readInt(ifs);
        i = readInt(ifs);
        mazeItem tp = makeItem(ITEM_TELEPORT);
        tp.teleport.x = i%w;
        tp.teleport.y = i/w;
        tp.teleport.tpChar = tpChar;
        tp.teleport.warpX = warp<0 ? -1 : warp%w;
        tp.teleport.warpY = warp<0 ? -1 : warp/w;
        m->addItem(tp);
        break;
    }
    default:
        return false;
    }
    return true;
}

std::unique_ptr<maze> openMaze(const char *filename) {
	std::ifstream ifs(filename, std::ifstream::in | std::ifstream::binary);

	ifs.seekg(0, std::ios::end);
	int filesize = (int) ifs.tellg();
	ifs.seekg(0, std::ios::beg);

    if (readInt(ifs) != MAGIC_NUMBER)
        return nullptr;

    int version = readInt(ifs);
    switch (version) {
    case 0x01000000:
    case 0x02000000:
        break;
    default:
        return nullptr;
    }

    int w = readShort(ifs);
    int h = readShort(ifs);

	std::unique_ptr<maze> m(new maze(w, h));
    m->version = version;

    while (ifs.tellg() < filesize) {
        if (!readFrame(m.get(), ifs)) {
            return nullptr;
        }
    }

	return m;
}
