#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "maze.h"
#include "a_star.h"
#include "sound.h"

struct camera {
	float yaw;
	float pitch;
	float roll;
	glm::vec3 position;

	glm::mat4 viewMatrix() const;
};

class gameLogic {
private:
    maze *m = nullptr;
	unique_ptr<a_star> pathfinder = nullptr;
    list<const node *> path;
    list<const node *>::iterator stepIndex;

	sound SND_TELEPORT;
	sound SND_HOLE;
	sound SND_WIN;

	music currentMusic;

	SDL_GameController *controller = nullptr;

private:
	int tickCount = 0;

	int tx = 0;
	int ty = 0;

	int tx_prev = 0;
	int ty_prev = 0;

	int angleFacing = 0;
	int lastMoveAngle = -1;

	int moving = 0;
	int restartDelay = 0;

	bool teleported = false;
	bool won = false;

	camera cam;

	bool lockToMarble = false;

	glm::vec3 marblePos;
	glm::mat4 marbleRotation;

	float startX = 0.f;
	float startZ = 0.f;
	float fallSpeed = 0.f;

private:
    void rollMarble(glm::vec3 delta);
    void teleportTo(int _x, int _y);
    bool offsetMove(int angleOffset);
	bool canMove(int moveAngle, bool checkBlocks = false);

    void startPathing();
    void nextStep();

	bool useItem();
	void endMove();
	void teleportToStart(bool resetWon);

	void setupCamera();
	void setupMarble();

	void loadMusic();

public:
    gameLogic(maze *m);
	virtual ~gameLogic();

public:
	bool init();

    void tick();
	void handleEvent(SDL_Event &e) {}

	bool hasTeleported() {
		return teleported;
	}

	bool hasWon() {
		return won;
	}

	const camera &getCamera() {
		return cam;
	}

	glm::mat4 marbleMatrix() {
		glm::mat4 m_position;
		m_position = glm::translate(m_position, marblePos);
		return m_position * marbleRotation;
	}
};

#endif // __GAME_LOGIC_H__
