#ifndef __NODE_H__
#define __NODE_H__

#include "tile.h"

static const int MAX_LINKS = 8;

enum algorithm {
	ALGORITHM_ASTAR,
	ALGORITHM_GBFS,
	ALGORITHM_DIJKSTRA,
};

extern algorithm currentAlgorithm;

struct link {
    class node *neighbour;
    int distance;
};

class node {
public:
    int gScore;
    int hScore;
    int x;
    int y;
    int z;

    bool closed;
    const node *parent;

    link links[MAX_LINKS];
    int linkCount;

public:
    node(int x = 0, int y = 0);
    node(const node&b);

    virtual ~node() {}

public:
    void reset();

    void addLink(node *neighbour, int distance = 1);

    int fScore() const;

    node &operator = (const node &b);
    bool operator < (const node &b) const;
    bool operator > (const node &b) const;
	bool operator == (const node &b) const;

    int heuristicDistance(const node &b) const;
};

#endif // __NODE_H__
