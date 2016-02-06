#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include <memory>
#include <glm/glm.hpp>
#include <sound.h>

#include "maze.h"
#include "a_star.h"

class gameLogic {
private:
    maze *m = nullptr;
	unique_ptr<a_star> pathfinder = nullptr;
    list<const node *> path;
    list<const node *>::iterator stepIndex;

	sound SND_TELEPORT;
	sound SND_HOLE;
	sound SND_WIN;

private:
	int tx = 0;
	int ty = 0;
	int startTx = 0;
	int startTy = 0;
	int angleFacing = 0;
	int lastMoveAngle = -1;
	bool lockToMarble = false;
	int moving = 0;
	int restartDelay = 0;

	bool teleported = false;
	bool won = false;

    glm::mat4 marbleRotation;

	float rotationY = 0.f;
	float cameraY = 0.f;

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float startX = 0.f;
	float startZ = 0.f;
	float fallSpeed = 0.f;

private:
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
