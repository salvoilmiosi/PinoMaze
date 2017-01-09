#include "game_logic.h"

#include "res_loader.h"
#include "globals.h"

glm::mat4 camera::viewMatrix() const {
	glm::mat4 m_position, m_pitch, m_yaw, m_roll;

	m_position = glm::translate(m_position, -position);
	m_pitch = glm::rotate(m_pitch, pitch, glm::vec3(1.f, 0.f, 0.f));
	m_yaw = glm::rotate(m_yaw, -yaw, glm::vec3(0.f, 1.f, 0.f));
	m_roll = glm::rotate(m_roll, -roll, glm::vec3(0.f, 0.f, 1.f));

	return m_roll * m_pitch * m_yaw * m_position;
}

void gameLogic::loadMusic() {
	static const int NUM_MUSIC = 24;

	static int i = rand() % NUM_MUSIC;
	static char MUSIC_ID[16];

	memset(MUSIC_ID, 0, 16);
	snprintf(MUSIC_ID, 16, "MUSIC_%d", i);

	i = (++i) % NUM_MUSIC;

	loadMusicFromResource(currentMusic, MUSIC_ID);
	currentMusic.play();
}

gameLogic::gameLogic(maze *m) : m(m) {}

gameLogic::~gameLogic() {
	currentMusic.stop();

	if (controller) {
		SDL_GameControllerClose(controller);
	}
}

bool gameLogic::init() {
	loadWaveFromResource(SND_TELEPORT, "IDW_TELEPORT");
	loadWaveFromResource(SND_HOLE, "IDW_HOLE");
	loadWaveFromResource(SND_WIN, "IDW_WIN");

	currentMusic.volume(musicVolume);
	loadMusic();

	teleportToStart(true);

	controller = SDL_GameControllerOpen(0);

	return true;
}

void gameLogic::teleportTo(int _x, int _y) {
    tx = _x;
    ty = _y;
	restartDelay = 0;
	moving = 0;

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

	marblePos = { (tx + 0.5f) * tileSize, marbleRadius, (ty + 0.5f) * tileSize };

	cam.yaw = angleFacing * M_PI / 2.f;
	cam.position = marblePos;
    cam.position.y = cameraHeight;

	endMove();

	teleported = true;
	lockToMarble = false;
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
		tx_prev = tx;
		ty_prev = ty;
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
		startX = marblePos.x;
		startZ = marblePos.z;
		moving = ticksPerMove;

		return true;
	}

	return false;
}

void gameLogic::setupCamera() {
	cam.position.x = marblePos.x;
	cam.position.z = marblePos.z;

    float angleDiff = angleFacing * M_PI / 2.f - cam.yaw;
    if (angleDiff > M_PI) {
        angleDiff -= M_PI * 2.f;
    }
    if (angleDiff < -M_PI) {
        angleDiff += M_PI * 2.f;
    }
    cam.yaw += angleDiff * cameraTurnSpeed;
    if (cam.yaw < 0.f) {
		cam.yaw += M_PI * 2.f;
    }
    if (cam.yaw >= M_PI * 2.f) {
		cam.yaw -= M_PI * 2.f;
    }

	tile *tile_from = m->getTile(tx_prev, ty_prev);
	tile *tile_to = m->getTile(tx, ty);

	float cameraToY = cameraHeight;

	if (tile_from->state == STATE_ITEM || tile_to->state == STATE_ITEM) {
		mazeItem *item_from = m->findItem(tile_from);
		mazeItem *item_to = m->findItem(tile_to);

		if ((lastMoveAngle == 0 || lastMoveAngle == 2)
			&& ((item_to && item_to->type == ITEM_BRIDGE)
				|| (item_from && item_from->type == ITEM_BRIDGE && moving >= ticksPerMove / 2))) {
			cameraToY = bridgeArcHeight * 0.4f;
		} else if ((item_to && item_to->type == ITEM_BRIDGE) || (item_from && item_from->type == ITEM_BRIDGE)) {
			cameraToY += marblePos.y - marbleRadius;
		}
	}

	float pitch = glm::radians(cameraPitch);

	float radius = cosf(pitch) * cameraDistance;
	float distX = sinf(cam.yaw) * radius;
	float distY = sinf(pitch) * cameraDistance;
	float distZ = cosf(cam.yaw) * radius;
	cam.position += glm::vec3(distX, 0.f, distZ);

	float dy = cameraToY + distY - cam.position.y;
	cam.position.y += dy * cameraSpeed;

	if (lockToMarble) {
		float cameraToPitch = atan2f(cam.position.y - marblePos.y, radius);
		float delta = cameraToPitch - cam.pitch;
		cam.pitch += delta * 0.05f;
	} else {
		cam.pitch = pitch;
	}
}

void gameLogic::setupMarble() {
	tile *tile_from = m->getTile(tx_prev, ty_prev);
	tile *tile_to = m->getTile(tx, ty);

	static const float l = tileSize - wallThickness;
	static const float l2 = l + bridgeArcThickness;
	static const float a = l2 / 2.f + marbleRadius;
	static const float b = l2 / l * bridgeArcHeight + marbleRadius;

	bool onItem = false;

	if (tile_from->state == STATE_ITEM || tile_to->state == STATE_ITEM) {
		mazeItem *item_from = m->findItem(tile_from);
		mazeItem *item_to = m->findItem(tile_to);

		if (moving > 0 && (lastMoveAngle == 1 || lastMoveAngle == 3)) {
			if (item_from && item_from->type == ITEM_BRIDGE) {
				float dx = abs(marblePos.x - startX);
				if (dx < a) {
					dx += a;
					marblePos.y = MAX(b / a * sqrtf(dx * (2.f * a - dx)), marbleRadius);
				} else {
					marblePos.y = marbleRadius;
				}
			}
			if (item_to && item_to->type == ITEM_BRIDGE) {
				float dx = abs(marblePos.x - startX);
				dx -= tileSize - a;
				if (dx > 0.f) {
					marblePos.y = MAX(b / a * sqrtf(dx * (2.f * a - dx)), marblePos.y);
				} else {
					marblePos.y = MAX(marbleRadius, marblePos.y);
				}
			}
		}

		onItem = true;
	}
	
	if (tile_to->state == STATE_BLOCK) {
		if (moving <= ticksPerMove / 2) {
			marblePos.y -= fallSpeed;
			fallSpeed += gravityAccel;
			lockToMarble = true;
		} else {
			fallSpeed = 0.f;
		}
	} else if (!onItem) {
		marblePos.y = marbleRadius;
	}
}

void gameLogic::rollMarble(glm::vec3 delta) {
	float rollAmount = 1.f / marbleRadius / M_PI;

    float angleX = (delta.z != 0.f) ? sqrtf(delta.z*delta.z + delta.y*delta.y) * rollAmount : 0.f;
	if (delta.z < 0.f) angleX = -angleX;

    float angleZ = (delta.x != 0.f) ? sqrtf(delta.x*delta.x + delta.y*delta.y) * rollAmount : 0.f;
	if (delta.x < 0.f) angleZ = -angleZ;

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
		int newAngle = 0;
        if (dx == 0) {
            if (dy > 0) {
				newAngle = 2;
            } else if (dy < 0) {
				newAngle = 0;
            }
        } else if (dy == 0) {
            if (dx > 0) {
				newAngle = 3;
            } else if (dx < 0) {
				newAngle = 1;
            }
        }
		angleFacing = newAngle;
		if (!offsetMove(newAngle - angleFacing)) {
			pathfinder = nullptr;
		}
    }
}

bool gameLogic::useItem() {
	tile *t = m->getTile((short)(marblePos.x/tileSize), (short)(marblePos.z/tileSize));
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
	++tickCount;
	teleported = false;

	if (moving > 0) {
		float endX = (tx + 0.5f) * tileSize;
		float endZ = (ty + 0.5f) * tileSize;

		--moving;

		marblePos.x = endX - (endX - startX) * ((float)moving / ticksPerMove);
		marblePos.z = endZ - (endZ - startZ) * ((float)moving / ticksPerMove);

		if (moving == 0) {
			endMove();
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	static bool pressed_space = false;

	bool up, down, left, right, strafeleft, straferight, use, reset, autoplay, changemusic;

	up = (keys[SDL_SCANCODE_UP] != 0) || (keys[SDL_SCANCODE_W] != 0);
	down = (keys[SDL_SCANCODE_DOWN] != 0) || (keys[SDL_SCANCODE_S] != 0);
	left = keys[SDL_SCANCODE_LEFT] != 0;
	right = keys[SDL_SCANCODE_RIGHT] != 0;
	strafeleft = keys[SDL_SCANCODE_A] != 0;
	straferight = keys[SDL_SCANCODE_D] != 0;
	use = keys[SDL_SCANCODE_SPACE] != 0;
	reset = keys[SDL_SCANCODE_R] != 0;
	autoplay = keys[SDL_SCANCODE_P] != 0;
	changemusic = keys[SDL_SCANCODE_M] != 0;

	if (restartDelay > 0) {
		--restartDelay;
		if (moving == 0) {
			// Keep moving forward when falling
			float move = tileSize / ticksPerMove * MAX((1 - powf((fallingDelay - restartDelay) / 10.f, 2.f)), 0.f);
			switch (lastMoveAngle) {
			case 0:
				marblePos.z -= move;
				break;
			case 1:
				marblePos.x -= move;
				break;
			case 2:
				marblePos.z += move;
				break;
			case 3:
				marblePos.x += move;
				break;
			}
		}
		if (restartDelay == 0) {
			teleportToStart(false);
			SND_HOLE.play();
		}
	} else {
		if (controller) {
			use = use || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
			reset = reset || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
			autoplay = autoplay || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
			changemusic = changemusic || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK);

			static const short sens = 20000;

			short l_x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
			strafeleft = strafeleft || (l_x < -sens);
			straferight = straferight || (l_x > sens);

			short l_y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
			up = up || (l_y < -sens);
			down = down || (l_y > sens);

			short r_x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
			left = left || (r_x < -sens);
			right = right || (r_x > sens);

			up = up || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
			down = down || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			left = left || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			right = right || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		}

		if (up) {
			offsetMove(0);
			pathfinder = nullptr;
		} else if (down) {
			offsetMove(2);
			pathfinder = nullptr;
		}

		if (use) {
			if (!pressed_space && useItem()) {
				pressed_space = true;
			}
		} else {
			pressed_space = false;
		}

		if (strafeleft) {
			offsetMove(1);
			pathfinder = nullptr;
		} else if (straferight) {
			offsetMove(3);
			pathfinder = nullptr;
		}
	}

	static bool pressed_p = false;
	if (autoplay) {
		if (!pressed_p) {
			startPathing();
			pressed_p = true;
		}
	} else {
		pressed_p = false;
	}

	static bool pressed_r = false;
	if (reset) {
		if (!pressed_r) {
			pathfinder = nullptr;
			teleportToStart(true);
			pressed_r = true;
		}
	} else {
		pressed_r = false;
	}

	static bool pressed_m = false;
	if (changemusic) {
		if (!pressed_m) {
			loadMusic();
			pressed_m = true;
		}
	} else {
		pressed_m = false;
	}

    static bool leftPressed = false;
    if (left) {
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
    if (right) {
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

	static glm::vec3 lastPos = marblePos;

	rollMarble(marblePos - lastPos);

    setupMarble();
	setupCamera();

	lastPos = marblePos;
}
