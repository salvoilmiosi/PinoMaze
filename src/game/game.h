#ifndef __GAME_H__
#define __GAME_H__

#include "maze.h"
#include "a_star.h"

#include "engine/context.h"

#include "render/material.h"

#include <glm/glm.hpp>
#include <memory>

struct camera {
	float yaw{0.f};
	float pitch{0.f};
	float roll{0.f};
	glm::vec3 position{0.f};
};

class game {
public:
    game(context *m_context, maze *m_maze);
    ~game();

public:
    void tick();
	void updateMatrices(float deltaMs);
	void handleEvent(SDL_Event &event);

public:
    maze *m_maze;

	glm::mat4 m_view;
	glm::mat4 m_proj;
	glm::mat4 m_marble;

	camera m_camera;

    light sun;

	int teleportTimer = 0;
	bool won = false;

private:
	context *m_context;

	std::unique_ptr<a_star> pathfinder = nullptr;
    std::list<const node *> path;
    std::list<const node *>::iterator stepIndex;

	sound SND_TELEPORT;
	sound SND_HOLE;
	sound SND_WIN;

	music currentMusic;

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

	bool lockToMarble = false;

	glm::vec3 marblePos;
	glm::mat4 marbleRotation{1.f};

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

	void setupCamera(float deltaMs);
	void setupMarble(float deltaMs);

	void loadMusic();
};

#endif