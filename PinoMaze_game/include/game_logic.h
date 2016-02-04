#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include <memory>
#include <glm/glm.hpp>

#include "maze.h"
#include "a_star.h"

class gameLogic {
private:
    maze *m;
	unique_ptr<a_star> pathfinder;
    list<const node *> path;
    list<const node *>::iterator stepIndex;

private:
    int tx, ty;
    int startTx, startTy;
    int angleFacing;
    int lastMoveAngle;
	bool lockToMarble;
    int moving;
	int restartDelay;

	bool teleported;
	bool won;

    glm::mat4 marbleRotation;

    float rotationY;
	float cameraY;

    float x, y, z;
    float startX, startZ;
	float fallSpeed;

    void rollMarble(float ox, float oy, float oz);
    void teleportTo(int _x, int _y);
    bool offsetMove(int angleOffset);
	bool canMove(int moveAngle, bool checkBlocks = false);

    void startPathing();
    void nextStep();

	bool useItem();
	void endMove();
	void teleportToStart(bool resetWon);

    void calculateRotation();
    void calculateHeight();

public:
    gameLogic();

public:
    void setMaze(maze *m);

    void tick();
	void handleEvent(SDL_Event &e);

	bool hasTeleported() {
		return teleported;
	}

	bool hasWon() {
		return won;
	}

	void resetTeleported() {
		teleported = false;
	}

    glm::mat4 viewMatrix();
    glm::mat4 marbleMatrix();
};

#endif // __GAME_LOGIC_H__
