#include "game_logic.h"

#include <glm/gtc/matrix_transform.hpp>

#include "res_loader.h"
#include "globals.h"

gameLogic::gameLogic(maze *m) : m(m) {
}

gameLogic::~gameLogic() {
	MUS_MUSIC.stop();
}

bool gameLogic::init() {
	SND_TELEPORT.loadChunk(loadWaveFromResource("IDW_TELEPORT"));
	SND_HOLE.loadChunk(loadWaveFromResource("IDW_HOLE"));
	SND_WIN.loadChunk(loadWaveFromResource("IDW_WIN"));
	MUS_MUSIC.loadMusic(loadMusicFromResource("IDM_TEMA1"));

	MUS_MUSIC.volume(0.3f);
	MUS_MUSIC.fadeIn(1000);

	teleportToStart(true);

	return true;
}

void gameLogic::teleportTo(int _x, int _y) {
    tx = _x;
    ty = _y;
	restartDelay = 0;

	int a;
	for (int angle = angleFacing; angle - angleFacing < 4; ++angle) {
		a = angle;
		if (a > 3) {
			a -= 4;
		}
		if (canMove(a, true)) {
			angleFacing = a;
			break;
		}
	}

    rotationY = angleFacing * (float) M_PI / 2.f;
    x = (tx + 0.5f) * tileSize;
    y = marbleRadius;
    z = (ty + 0.5f) * tileSize;

    cameraY = cameraHeight;
    moving = 0;

	endMove();

	teleported = true;
	lockToMarble = false;
}

glm::mat4 gameLogic::viewMatrix() {
	glm::mat4 camera, pitch, yaw, distance;

	static float anglePitch = glm::radians(cameraPitch);
	static float c_distanceY;
	static float c_distanceZ;
	if (lockToMarble) {
		c_distanceZ = cosf(glm::radians(cameraPitch)) * cameraDistance;
		c_distanceY = sinf(glm::radians(cameraPitch)) * cameraDistance;
		float a = c_distanceZ;
		float b = c_distanceY + cameraY - y;
		float angleToMarble = atan2f(b, a);
		float offset = angleToMarble - anglePitch;
		anglePitch += offset * 0.05f;
	} else {
		anglePitch = glm::radians(cameraPitch);
		c_distanceY = 0.f;
		c_distanceZ = cameraDistance;
	}

	camera = glm::translate(camera, glm::vec3(-x, -cameraY, -z));
	yaw = glm::rotate(yaw, rotationY, glm::vec3(0.f, -1.f, 0.f));
	distance = glm::translate(distance, glm::vec3(0.f, -c_distanceY, -c_distanceZ));
	pitch = glm::rotate(pitch, anglePitch, glm::vec3(1.f, 0.f, 0.f));

	if (lockToMarble) {
		return pitch * distance * yaw * camera;
	} else {
		return distance * pitch * yaw * camera;
	}
}

glm::mat4 gameLogic::marbleMatrix() {
    glm::mat4 position;
    position = glm::translate(position, glm::vec3(x, y, z));
    return position * marbleRotation;
}

bool gameLogic::canMove(int moveAngle, bool checkBlocks) {
	int ox = 0;
	int oy = 0;

	switch (moveAngle) {
	case 0:
		--oy;
		break;
	case 1:
		--ox;
		break;
	case 2:
		++oy;
		break;
	case 3:
		++ox;
		break;
	}

	tile *curTile = m->getTile(tx, ty);
	tile *nextTile = m->getTile(tx + ox, ty + oy);
	if (!nextTile) return false;

	if (checkBlocks && nextTile->state == STATE_BLOCK) {
		return false;
	}

	if (ox > 0 && m->vwalls[tx + 1][ty]) return false;
	if (ox < 0 && m->vwalls[tx][ty]) return false;
	if (oy > 0 && m->hwalls[ty + 1][tx]) return false;
	if (oy < 0 && m->hwalls[ty][tx]) return false;

	if (curTile->state == STATE_ITEM) {
		mazeItem *item = m->findItem(curTile);
		if (item) {
			switch (item->type) {
			case ITEM_BRIDGE:
				if (lastMoveAngle != -1 && lastMoveAngle != moveAngle) {
					int backAngle = lastMoveAngle + 2;
					if (backAngle > 3) backAngle -= 4;
					if (backAngle != moveAngle) {
						return false;
					}
				}
				break;
			case ITEM_ARROW:
				if (!arrowPointsTo(*item, ox, oy)) {
					return false;
				}
				break;
			}
		}
	}

	return true;
}

bool gameLogic::offsetMove(int angleOffset) {
    if (moving > 0) return false;

    int moveAngle = angleFacing + angleOffset;
    if (moveAngle > 3) {
        moveAngle -= 4;
    }
	if (canMove(moveAngle)) {
		lastMoveAngle = moveAngle;
		startTx = tx;
		startTy = ty;
		switch (moveAngle) {
		case 0:
			--ty;
			break;
		case 1:
			--tx;
			break;
		case 2:
			++ty;
			break;
		case 3:
			++tx;
			break;
		}
		startX = x;
		startZ = z;
		moving = ticksPerMove;

		return true;
	}

	return false;
}

void gameLogic::calculateRotation() {
    float angleDiff = angleFacing * (float) M_PI / 2.f - rotationY;
    if (angleDiff > M_PI) {
        angleDiff -= (float) M_PI * 2.f;
    }
    if (angleDiff < -M_PI) {
        angleDiff += (float) M_PI * 2.f;
    }
    rotationY += angleDiff * cameraTurnSpeed;
    if (rotationY < 0.f) {
        rotationY += (float) M_PI * 2.f;
    }
    if (rotationY >= (float) M_PI * 2.f) {
        rotationY -= (float) M_PI * 2.f;
    }
}

void gameLogic::calculateHeight() {
	tile *tile_from = m->getTile(startTx, startTy);
	tile *tile_to = m->getTile(tx, ty);

	static const float l = tileSize - wallThickness;
	static const float l2 = l + bridgeArcThickness;
	static const float a = l2 / 2.f + marbleRadius;
	static const float b = l2 / l * bridgeArcHeight + marbleRadius;

	float cameraToY = cameraHeight;
	bool onItem = false;

	if (tile_from->state == STATE_ITEM || tile_to->state == STATE_ITEM) {
		mazeItem *item_from = m->findItem(tile_from);
		mazeItem *item_to = m->findItem(tile_to);

		if (moving > 0 && (lastMoveAngle == 1 || lastMoveAngle == 3)) {
			if (item_from && item_from->type == ITEM_BRIDGE) {
				float dx = abs(x - startX);
				if (dx < a) {
					dx += a;
					y = MAX(b / a * sqrtf(dx * (2.f * a - dx)), marbleRadius);
				} else {
					y = marbleRadius;
				}
			}
			if (item_to && item_to->type == ITEM_BRIDGE) {
				float dx = abs(x - startX);
				dx -= tileSize - a;
				if (dx > 0.f) {
					y = MAX(b / a * sqrtf(dx * (2.f * a - dx)), y);
				} else {
					y = MAX(marbleRadius, y);
				}
			}
		}

		if ((lastMoveAngle == 0 || lastMoveAngle == 2)
			&& ((item_to && item_to->type == ITEM_BRIDGE)
			|| (item_from && item_from->type == ITEM_BRIDGE && moving >= ticksPerMove / 2))) {
			cameraToY = bridgeArcHeight * 0.4f;
		} else if ((item_to && item_to->type == ITEM_BRIDGE) || (item_from && item_from->type == ITEM_BRIDGE)) {
			cameraToY += y - marbleRadius;
		}
		onItem = true;
	}
	
	if (tile_to->state == STATE_BLOCK) {
		if (moving <= ticksPerMove / 2) {
			y -= fallSpeed;
			fallSpeed += gravityAccel;
			lockToMarble = true;
		} else {
			fallSpeed = 0.f;
		}
	} else if (!onItem) {
		y = marbleRadius;
	}

	float dy = cameraToY - cameraY;
	cameraY += dy * cameraSpeed;
}

void gameLogic::rollMarble(float ox, float oy, float oz) {
	float rollAmount = 1.f / marbleRadius / (float) M_PI;

    float angleX = (oz != 0.f) ? sqrtf(oz*oz + oy*oy) * rollAmount : 0.f;
	if (oz < 0.f) angleX = -angleX;

    float angleZ = (ox != 0.f) ? sqrtf(ox*ox + oy*oy) * rollAmount : 0.f;
	if (ox < 0.f) angleZ = -angleZ;

    glm::mat4 rotation;
    rotation = glm::rotate(rotation, angleX, glm::vec3( 1.f, 0.f, 0.f));
	rotation = glm::rotate(rotation, angleZ, glm::vec3(0.f, 0.f, -1.f));

    marbleRotation = rotation * marbleRotation;
}

void gameLogic::startPathing() {
	pathfinder.reset(new a_star);
    pathfinder->setMaze(m);

	int tz = 0;
	tile *t;
	mazeItem *item;
	if ((t = m->getTile(tx, ty)) && (t->state == STATE_ITEM) && (item = m->findItem(t)) && (item->type == ITEM_BRIDGE)) {
		if (lastMoveAngle == 1 || lastMoveAngle == 3) {
			tz = 1;
		}
	}
	pathfinder->setStart(tx, ty, tz);
    path = pathfinder->getPath();

    if (path.empty()) {
		pathfinder = nullptr;
    } else {
        stepIndex = path.begin();
		if (moving == 0) {
			nextStep();
		}
    }
}

void gameLogic::nextStep() {
    const node *current = *stepIndex;
    ++stepIndex;
    if (pathfinder->getEndNode() == current) {
		pathfinder = nullptr;
        return;
    }
    const node *next = *stepIndex;
    int dx = next->x - current->x;
    int dy = next->y - current->y;
    if (abs(dx)+abs(dy) > 1) {
		if (!useItem()) {
			pathfinder = nullptr;
		}
    } else {
        if (dx == 0) {
            if (dy > 0) {
                angleFacing = 2;
            } else if (dy < 0) {
                angleFacing = 0;
            }
        } else if (dy == 0) {
            if (dx > 0) {
                angleFacing = 3;
            } else if (dx < 0) {
                angleFacing = 1;
            }
        }
		if (!offsetMove(0)) {
			pathfinder = nullptr;
		}
    }
}

bool gameLogic::useItem() {
	tile *t = m->getTile((short)(x/tileSize), (short)(z/tileSize));
	if (t) {
		mazeItem *item = m->findItem(t);
		if (item) {
			switch (item->type) {
			case ITEM_TELEPORT:
				if (item->teleport.warpX >= 0 && item->teleport.warpY >= 0) {
					teleportTo(item->teleport.warpX, item->teleport.warpY);
					SND_TELEPORT.play();
					return true;
				}
				break;
			}
		}
	}

	return false;
}

void gameLogic::endMove() {
	tile *t = m->getTile(tx, ty);
	if (t) {
		switch (t->state) {
		case STATE_BLOCK:
			restartDelay = fallingDelay;
			break;
		case STATE_END:
			if (!won) {
				won = true;
				SND_WIN.play();
				MUS_MUSIC.fadeOut(3000);
			}
			break;
		}
	}

	if (pathfinder) {
		nextStep();
	}
}

void gameLogic::teleportToStart(bool resetWon) {
	tile *startTile = m->startTile();
	int x = 0;
	int y = 0;
	if (startTile) {
		int index = m->getIndex(startTile);
		x = index % m->width();
		y = index / m->width();
	}
	teleportTo(x, y);

	if (resetWon) {
		won = false;
	}
}

void gameLogic::tick() {
	teleported = false;

	if (moving > 0) {
		float endX = (tx + 0.5f) * tileSize;
		float endZ = (ty + 0.5f) * tileSize;

		--moving;

		x = endX - (endX - startX) * ((float)moving / ticksPerMove);
		z = endZ - (endZ - startZ) * ((float)moving / ticksPerMove);

		if (moving == 0) {
			endMove();
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	static bool pressed_space = false;

	if (restartDelay > 0) {
		--restartDelay;
		if (moving == 0) {
			// Keep moving forward when falling
			float move = tileSize / ticksPerMove * MAX((1 - powf((fallingDelay - restartDelay) / 10.f, 2.f)), 0.f);
			switch (lastMoveAngle) {
			case 0:
				z -= move;
				break;
			case 1:
				x -= move;
				break;
			case 2:
				z += move;
				break;
			case 3:
				x += move;
				break;
			}
		}
		if (restartDelay == 0) {
			teleportToStart(false);
			SND_HOLE.play();
		}
	} else {
		if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
			offsetMove(0);
			pathfinder = nullptr;
		} else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
			offsetMove(2);
			pathfinder = nullptr;
		}

		if (keys[SDL_SCANCODE_SPACE]) {
			if (!pressed_space && useItem()) {
				pressed_space = true;
			}
		} else {
			pressed_space = false;
		}

		if (keys[SDL_SCANCODE_A]) {
			offsetMove(1);
			pathfinder = nullptr;
		} else if (keys[SDL_SCANCODE_D]) {
			offsetMove(3);
			pathfinder = nullptr;
		}
	}

	static bool pressed_p = false;
	if (keys[SDL_SCANCODE_P]) {
		if (!pressed_p) {
			startPathing();
			pressed_p = true;
		}
	} else {
		pressed_p = false;
	}

	static bool pressed_r = false;
	if (keys[SDL_SCANCODE_R]) {
		if (!pressed_r) {
			pathfinder = nullptr;
			teleportToStart(true);
			pressed_r = true;
			MUS_MUSIC.fadeIn(1000);
		}
	} else {
		pressed_r = false;
	}

    static bool leftPressed = false;
    if (keys[SDL_SCANCODE_LEFT]) {
        if (!leftPressed) {
            ++angleFacing;
            if (angleFacing > 3) {
                angleFacing = 0;
            }
            leftPressed = true;
        }
    } else {
        leftPressed = false;
    }
    static bool rightPressed = false;
    if (keys[SDL_SCANCODE_RIGHT]) {
        if (!rightPressed) {
            --angleFacing;
            if (angleFacing < 0) {
                angleFacing = 3;
            }
            rightPressed = true;
        }
    } else {
        rightPressed = false;
    }

	static float lastX = x;
	static float lastY = y;
	static float lastZ = z;

	rollMarble(x - lastX, y - lastY, z - lastZ);

    calculateRotation();
    calculateHeight();

	lastX = x;
	lastY = y;
	lastZ = z;
}

void gameLogic::handleEvent(SDL_Event & e) {

}
