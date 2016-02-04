#include "maze_random.h"

#include "maze_items.h"

#include <cstdlib>

static void randomWall (wall &w, int probability) {
    for (int i=0; i < w.length(); ++i) {
        w[i] = rand() % 100 < probability;
    }
}

unique_ptr<maze> generateRandomMaze(int w, int h) {
	unique_ptr<maze> m(new maze(w, h));

    for (int i=0; i<w*h; ++i)  {
        m->getTile(i)->state = rand() % 100 >= BLOCK_PROBABILITY ? STATE_FLOOR : STATE_BLOCK;
    }

    for (vector<wall>::iterator i = m->hwalls.begin(); i != m->hwalls.end(); ++i) {
        randomWall(*i, WALL_PROBABILITY_H);
    }

    for (vector<wall>::iterator i = m->vwalls.begin(); i != m->vwalls.end(); ++i) {
        randomWall(*i, WALL_PROBABILITY_V);
    }

    m->getTile(rand() % w, rand() % h)->state = STATE_START;
    m->getTile(rand() % w, rand() % h)->state = STATE_END;

    int numTeleports = (rand() % 10 + 3) * 2;

    mazeItem currentTp = makeItem();
    mazeItem newTp = makeItem();

    while(numTeleports>0) {
        int tpX = rand() % w;
        int tpY = rand() % h;

        tile *tpTile = m->getTile(tpX, tpY);
        if (tpTile->state != STATE_FLOOR)
            continue;

        tpTile->state = STATE_ITEM;
        newTp = makeItem(ITEM_TELEPORT);
        newTp.teleport.x = tpX;
        newTp.teleport.y = tpY;

        if (currentTp.item.type != ITEM_GENERIC) {
            currentTp.teleport.tpChar = rand()%0xfe + 1;
            newTp.teleport.tpChar = currentTp.teleport.tpChar;

            currentTp.teleport.warpX = tpX;
            currentTp.teleport.warpY = tpY;
            newTp.teleport.warpX = currentTp.teleport.x;
            newTp.teleport.warpY = currentTp.teleport.y;

            m->addItem(currentTp);
            m->addItem(newTp);

            currentTp.item.type = ITEM_GENERIC;
        } else {
            currentTp = newTp;
        }

        --numTeleports;
    }

    return m;
}
