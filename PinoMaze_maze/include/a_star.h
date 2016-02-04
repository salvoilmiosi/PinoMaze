#ifndef __A_STAR_H__
#define __A_STAR_H__

#include <SDL2/SDL.h>

#include "maze.h"

#include "grid3d.h"
#include "node.h"

#include "heap.h"

#include <list>

struct lessFunc_pt {
	bool operator()(node *a, node *b) {
		return *a > *b;
	}
};

class a_star : public grid3d < node > {
private:
	maze *m = nullptr;

public:
	a_star() : a_star(0, 0) {}

	a_star(int w, int h);

public:
	void setMaze(maze *m);

	void start();
	void tick();
	void stop();

	list<const node *> getPath();

	node *getItem(int x, int y, int z) = delete;
	node *getItem(vector3d coords) = delete;

	node *getNode(int x, int y, int z = 0) {
		return grid3d::getItem(x, y, z);
	}

	node *getNode(vector3d coords) {
		return grid3d::getItem(coords);
	}

	void setStart(int x, int y, int z = 0) {
		startCoords = { x, y, z };
	}

	void setEnd(int x, int y, int z = 0) {
		endCoords = { x, y, z };
	}

	node *getStartNode() {
		return startNode;
	}

	node *getEndNode() {
		return endNode;
	}

private:
	tile *findTile(const vector3d &vec);

	void resetNodes();

	void addNeighbour(node *ne, int ox, int oy);
	void findNeighbours();

	bool finding = false;
	bool found = false;

	list<const node *> path;

	vector3d startCoords = { -1 };
	vector3d endCoords = { -1 };

	node *startNode = nullptr;
	node *endNode = nullptr;

	heap<node *, lessFunc_pt> openSet;
	void pathfind();
};

#endif // __A_STAR_H__
