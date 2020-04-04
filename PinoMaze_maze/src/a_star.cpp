#include "a_star.h"

#include "maze_items.h"

a_star::a_star(int w, int h) : grid3d<node>(w, h, 2), openSet(lessFunc_pt()) {}

std::list<const node *> a_star::getPath() {
    if (!found) {
        start();
        while (finding) {
            pathfind();
        }
    }
    return path;
}

void a_star::start() {
    if (m == nullptr) return;

    resetNodes();

	tile *startTile = nullptr;
	tile *endTile = nullptr;

	if (startCoords.x < 0 || startCoords.y < 0 || startCoords.z < 0) {
		startTile = m->startTile();
		if (startTile) {
			int index = m->getIndex(startTile);
			setStart(index % m->width(), index / m->width());
		}
	} else {
		startTile = m->getTile(startCoords.x, startCoords.y);
	}

	if (endCoords.x < 0 || endCoords.y < 0 || endCoords.z < 0) {
		endTile = m->endTile();
		if (endTile) {
			int index = m->getIndex(endTile);
			setEnd(index % m->width(), index / m->width());
		}
	} else {
		endTile = m->getTile(endCoords.x, endCoords.y);
	}

	startNode = getNode(startCoords);
	endNode = getNode(endCoords);

	if (startTile) startTile->temp = TEMP_START;
    if (endTile) endTile->temp = TEMP_END;

    finding = startNode != nullptr;
    if (finding) {
        findNeighbours();
        openSet.clear();
        openSet.insert(startNode);
        path.clear();
	}

	startCoords = { -1 };
	endCoords = { -1 };
}

void a_star::stop() {
    finding = false;
    found = false;
    openSet.clear();
}

void a_star::setMaze(maze *_m) {
    m = _m;

    resetGrid(m->width(), m->height(), 2);
    stop();
}

void a_star::resetNodes() {
    for (int i=0; i < datasize(); ++i) {
        node *n = data + i;

        vector3d vec = getCoords(n);
        n->reset();
        n->x = vec.x;
        n->y = vec.y;
        n->z = vec.z;

        tile *t = findTile(vec);

        if (t != nullptr) {
            if (t->temp == TEMP_PATH) {
                t->temp = TEMP_PATH_OLD;
            } else {
                t->temp = TEMP_NULL;
            }
        }
    }
    startNode = nullptr;
    endNode = nullptr;
    finding = false;
}

tile *a_star::findTile(const vector3d &vec) {
    tile *t = m->getTile(vec.x, vec.y);

    if (vec.z == 0) {
        return t;
    } else {
        mazeItem *it = m->findItem(t);
        if (it != nullptr && it->type == ITEM_BRIDGE) {
            return &(it->bridge.upperTile);
        }
    }
    return nullptr;
}

void a_star::tick() {
    if (finding) {
        pathfind();

        for (int i=0; i<w*h*_depth; ++i) {
            node *n = data + i;
            vector3d vec = getCoords(n);

            tile *t = findTile(vec);

            if (t != nullptr) {
                if (openSet.contains(n) && t->temp == TEMP_NULL) {
                    t->temp = TEMP_NODE_OPEN;
                } else if (n->closed && t->temp == TEMP_NODE_OPEN) {
                    t->temp = TEMP_NODE_CLOSED;
                }
            }
        }
    }
    if (found) {
        for (const node *n : path) {
            tile *t = findTile(getCoords(n));
            if (n == endNode) {
                found = false;
                break;
            }
            if (t->temp != TEMP_PATH && t->temp != TEMP_START && t->temp != TEMP_END) {
                t->temp = TEMP_PATH;
                break;
            }
        }
    }
}

void a_star::addNeighbour(node *n, int ox, int oy) {
    tile *t = m->getTile(n->x, n->y);

    int nx = n->x + ox;
    int ny = n->y + oy;
    tile *neighbourTile = m->getTile(nx, ny);
    node *neighbourNode = getNode(nx, ny);

    mazeItem *it = m->findItem(t);
    if (it != nullptr) {
        switch (it->type) {
        case ITEM_BRIDGE:
            if (oy == 0) {
                n = getNode(n->x, n->y, 1);
            }
            break;
        case ITEM_ARROW:
            if (!arrowPointsTo(*it, ox, oy)) {
                return;
            }
            break;
        default:
            break;
        }
    }

    if (neighbourTile != nullptr && neighbourNode != nullptr) {
		mazeItem *it = m->findItem(neighbourTile);
		if (it != nullptr && it->type == ITEM_BRIDGE && oy == 0) {
			neighbourNode = getNode(nx, ny, 1);
		}

		n->addLink(neighbourNode, 1);
    }
}

void a_star::findNeighbours() {
	tile *tileStart = m->startTile();
	int index = m->getIndex(tileStart);
	int sx = index % m->width();
	int sy = index / m->width();
	node *nodeStart = getNode(sx, sy);

    for (node *n = data; n-data < datasize(); ++n) {
        vector3d vec = getCoords(n);
        if (vec.z != 0) continue;

        tile *t = m->getTile(vec.x, vec.y);

		if (t->state == STATE_BLOCK) {
			if (nodeStart) {
				n->addLink(nodeStart, 1);
			}
		} else {
			mazeItem *it = m->findItem(t);
			if (it != nullptr && it->type == ITEM_TELEPORT) {
				tile *warpTile = m->getTile(it->teleport.warpX, it->teleport.warpY);
				if (warpTile && warpTile->state != STATE_BLOCK) {
					n->addLink(getNode(it->teleport.warpX, it->teleport.warpY), 1);
				}
			}

			if (!m->vwalls[n->x][n->y])
				addNeighbour(n, -1, 0);
			if (!m->hwalls[n->y][n->x])
				addNeighbour(n, 0, -1);
			if (!m->vwalls[n->x + 1][n->y])
				addNeighbour(n, 1, 0);
			if (!m->hwalls[n->y + 1][n->x])
				addNeighbour(n, 0, 1);
		}
    }
}

void a_star::pathfind() {
    if (! openSet.empty()) {
        node *currentNode = nullptr;

        currentNode = openSet.removeFirst();
        currentNode->closed = true;

        if (currentNode == endNode) {
            found = true;
            finding = false;

            path.clear();
            for (const node *n = endNode; n != startNode; n = n->parent) {
                path.push_front(n);
            }
            path.push_front(startNode);

            return;
        }

        link *links = currentNode->links;
        int linkCount = currentNode->linkCount;

        for (link *l = links; l - links < linkCount; ++l) {
            node *nei = l->neighbour;

            if (nei->closed) {
                continue;
            }

            int gScoreNew = currentNode->gScore + l->distance;

            bool neighbour_notOpen = !openSet.contains(nei);

            if (gScoreNew < nei->gScore || neighbour_notOpen) {
                nei->parent = currentNode;
                nei->gScore = gScoreNew;
                if (endNode != nullptr) {
                    nei->hScore = nei->heuristicDistance(*endNode);
                }

                if (neighbour_notOpen) {
                    openSet.insert(nei);
				} else {
					openSet.update(nei);
				}
            }
        }
    } else {
        finding = false;
        found = false;
    }
}

