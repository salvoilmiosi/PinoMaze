#include "node.h"

#include <cstdlib>
#include <cstring>

algorithm currentAlgorithm = ALGORITHM_ASTAR;

node::node(int x, int y) :x(x), y(y) {
    reset();
}

node::node(const node &b) {
    *this = b;
}

void node::reset() {
    gScore = 0;
    hScore = 0;
    closed = false;
    parent = nullptr;
    linkCount = 0;
    memset(links, 0, sizeof links);
}

void node::addLink(node *neighbour, int distance) {
    if (neighbour) {
        links[linkCount].neighbour = neighbour;
        links[linkCount].distance = 1;
        ++linkCount;
    }
}

node &node::operator = (const node &b) {
    gScore = b.gScore;
    hScore = b.hScore;
    x = b.x;
    y = b.y;
    parent = b.parent;
    closed = b.closed;
    memcpy(links, b.links, MAX_LINKS);
    linkCount = b.linkCount;
    return *this;
}

bool node::operator < (const node &b) const {
    if (fScore() == b.fScore()) {
        return hScore < b.hScore;
    } else {
        return fScore() < b.fScore();
    }
}

bool node::operator > (const node &b) const {
    if (fScore() == b.fScore()) {
        return hScore > b.hScore;
    } else {
        return fScore() > b.fScore();
    }
}

int node::fScore() const {
    switch (currentAlgorithm) {
    case ALGORITHM_GBFS:
        return hScore;
    case ALGORITHM_DIJKSTRA:
        return gScore;
    case ALGORITHM_ASTAR:
    default:
        return gScore + hScore;
    }
}

int node::heuristicDistance(const node &b) const {
    int dx = abs(b.x - x);
    int dy = abs(b.y - y);

    return dx+dy;
}

bool node::operator == (const node &b) const {
	return gScore == b.gScore &&
		hScore == b.hScore &&
		x == b.x &&
		y == b.y &&
		z == b.z;
}