#ifndef __GAME_H__
#define __GAME_H__

#include "maze.h"
#include "a_star.h"

#include "engine/entity.h"
#include "engine/material.h"
#include "engine/context.h"

struct camera {
	float yaw;
	float pitch;
	float roll;
	glm::vec3 position;
};

class game : public entity {
public:
    game(context *m_context, maze *m_maze);
    ~game();

public:
    void tick();

public:
    maze *m_maze;

	glm::mat4 m_view;
	glm::mat4 m_proj;
	glm::mat4 m_marble;

    light sun;

	int teleportTimer = 0;
	bool won = false;

private:
	std::unique_ptr<a_star> pathfinder = nullptr;
    std::list<const node *> path;
    std::list<const node *>::iterator stepIndex;

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

	camera cam;

	bool lockToMarble = false;

	glm::vec3 marblePos;
	glm::mat4 marbleRotation;

	float startX = 0.f;
	float startZ = 0.f;
	float fallSpeed = 0.f;

private:
	void handleInput();
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
};

#endif