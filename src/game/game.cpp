#include "game.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "resources.h"
#include "options.h"

game::game(engine *m_engine, maze &m_maze) : m_maze(m_maze), options(m_engine->options) {
    if (!materials::loadMaterials(BINARY_RESOURCE(materials_txt))) {
        throw std::string("Could not load materials");
    }
    
    m_proj = glm::perspective(glm::radians(options.fov), (float)options.window_width / (float)options.window_height, 0.1f, skyboxSize * 2.f);

	sun.direction = glm::vec3(0.43555f, 0.5f, -0.25391f);
    
	loadWaveFromResource(SND_TELEPORT, "IDW_TELEPORT");
	loadWaveFromResource(SND_HOLE, "IDW_HOLE");
	loadWaveFromResource(SND_WIN, "IDW_WIN");

	currentMusic.volume(musicVolume);

	teleportToStart(true);
}

game::~game() {
	materials::cleanupMaterials();
	currentMusic.stop();
}

void game::loadMusic() {
	static const int NUM_MUSIC = 14;

	static int i = rand() % NUM_MUSIC;
	static char MUSIC_ID[16];

	memset(MUSIC_ID, 0, 16);
	snprintf(MUSIC_ID, 16, "MUSIC_%d", i);

	i = (++i) % NUM_MUSIC;

	loadMusicFromResource(currentMusic, MUSIC_ID);
	currentMusic.play();
}

void game::teleportTo(int _x, int _y) {
    tx = tx_prev = _x;
    ty = ty_prev = _y;
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

	m_camera.yaw = angleFacing * M_PI / 2.f;
	m_camera.position = marblePos;
    m_camera.position.y = cameraHeight;

	endMove();

	teleportTimer = teleportTicks;
	falling = false;
}

bool game::canMove(int moveAngle, bool checkBlocks) {
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

	tile *curTile = m_maze.getTile(tx, ty);
	tile *nextTile = m_maze.getTile(tx + ox, ty + oy);
	if (!nextTile) return false;

	if (checkBlocks && nextTile->state == STATE_BLOCK) {
		return false;
	}

	if (ox > 0 && m_maze.vwalls[tx + 1][ty].solid()) return false;
	if (ox < 0 && m_maze.vwalls[tx][ty].solid()) return false;
	if (oy > 0 && m_maze.hwalls[ty + 1][tx].solid()) return false;
	if (oy < 0 && m_maze.hwalls[ty][tx].solid()) return false;

	if (curTile->state == STATE_ITEM) {
		mazeItem *item = m_maze.findItem(curTile);
		if (item) {
			switch (item->type) {
			case ITEM_BRIDGE:
				if (lastMoveAngle >= 0 && lastMoveAngle != moveAngle) {
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
			default:
				break;
			}
		}
	}

	return true;
}

bool game::offsetMove(int angleOffset) {
    if (moving > 0) return false;
	if (restartDelay > 0) return false;

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

float itemHeight(maze &m_maze, tile *t, float dist, int angle) {
	switch (t->state) {
	case STATE_START:
	case STATE_END:
		if (dist < startBoxSize / 2.f) {
			return marbleRadius + startBoxHeight;
		}
		break;
	case STATE_ITEM:
		if (mazeItem *item = m_maze.findItem(t)) {
			switch(item->type) {
			case ITEM_BRIDGE:
				if (angle % 2) {
					const float l = tileSize - wallThickness;
					const float l2 = l + bridgeArcThickness;
					const float a = l2 / 2.f + marbleRadius;
					const float b = l2 / l * bridgeArcHeight + marbleRadius;

					if (dist < a) {
						return b / a * sqrtf((dist + a) * (a - dist));
					}
				}
				break;
			case ITEM_TELEPORT:
				if (dist < teleportRadius2) {
					return marbleRadius + teleportHeight;
				} else {
					return marbleRadius + (teleportRadius1 - dist) * teleportHeight / (teleportRadius1 - teleportRadius2);
				}
				break;
			default:
				break;
			}
		}
	default:
		break;
	}
	return marbleRadius;
}

void game::setupMarble(float deltaNano) {
	tile *tile_from = m_maze.getTile(tx_prev, ty_prev);
	tile *tile_to = m_maze.getTile(tx, ty);
	
	float dist = (lastMoveAngle % 2) ? abs(marblePos.x - startX) : abs(marblePos.z - startZ);
	
	if (tile_to->state == STATE_BLOCK && moving <= ticksPerMove / 2) {
		marblePos.y -= fallSpeed * deltaNano / 1000000000.f;
		fallSpeed += gravityAccel * deltaNano / 1000000000.f;
		falling = true;
	} else {
		fallSpeed = 0.f;

		marblePos.y = MAX(marbleRadius, itemHeight(m_maze, tile_from, dist, lastMoveAngle));
		marblePos.y = MAX(marblePos.y, itemHeight(m_maze, tile_to, tileSize - dist, lastMoveAngle));
	}
	
    m_marble = glm::translate(glm::mat4(1.f), marblePos) * marbleRotation;
}

void game::rollMarble(glm::vec3 delta) {
	float rollAmount = 1.f / marbleRadius / M_PI;

    float angleX = (delta.z != 0.f) ? sqrtf(delta.z*delta.z + delta.y*delta.y) * rollAmount : 0.f;
	if (delta.z < 0.f) angleX = -angleX;

    float angleZ = (delta.x != 0.f) ? sqrtf(delta.x*delta.x + delta.y*delta.y) * rollAmount : 0.f;
	if (delta.x < 0.f) angleZ = -angleZ;

    glm::mat4 rotation(1.f);
    rotation = glm::rotate(rotation, angleX, glm::vec3( 1.f, 0.f, 0.f));
	rotation = glm::rotate(rotation, angleZ, glm::vec3(0.f, 0.f, -1.f));

    marbleRotation = rotation * marbleRotation;
}

void game::startPathing() {
	pathfinder.reset(new a_star);
    pathfinder->setMaze(&m_maze);

	int tz = 0;
	tile *t;
	mazeItem *item;
	if ((t = m_maze.getTile(tx, ty)) && (t->state == STATE_ITEM) && (item = m_maze.findItem(t)) && (item->type == ITEM_BRIDGE)) {
		if (lastMoveAngle % 2) {
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

void game::nextStep() {
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

bool game::useItem() {
	tile *t = (moving < ticksPerMove / 2) ? m_maze.getTile(tx, ty) : nullptr;
	if (t) {
		mazeItem *item = m_maze.findItem(t);
		if (item) {
			switch (item->type) {
			case ITEM_TELEPORT:
				if (item->teleport.warpX >= 0 && item->teleport.warpY >= 0) {
					teleportTo(item->teleport.warpX, item->teleport.warpY);
					SND_TELEPORT.play();
					return true;
				}
				break;
			default:
				break;
			}
		}
	}

	return false;
}

void game::endMove() {
	tile *t = m_maze.getTile(tx, ty);
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
		default:
			break;
		}
	}

	if (pathfinder) {
		nextStep();
	}
}

void game::teleportToStart(bool resetWon) {
	tile *startTile = m_maze.startTile();
	int x = 0;
	int y = 0;
	if (startTile) {
		int index = m_maze.getIndex(startTile);
		x = index % m_maze.width();
		y = index / m_maze.width();
	}
	teleportTo(x, y);

	if (resetWon) {
		won = false;
	}
}

void game::tick() {
	++tickCount;
	if (teleportTimer > 0) {
		--teleportTimer;
	}

	if (moving > 0) {
		--moving;

		if (moving == 0) {
			endMove();
		}
	}

	if (restartDelay > 0) {
		--restartDelay;
		if (restartDelay == 0) {
			teleportToStart(false);
			SND_HOLE.play();
		}
	}

	const Uint8 *key_down = SDL_GetKeyboardState(nullptr);
	
	if (key_down[SDL_SCANCODE_UP] || key_down[SDL_SCANCODE_W]) {
		offsetMove(0);
		pathfinder = nullptr;
	} else if (key_down[SDL_SCANCODE_DOWN] || key_down[SDL_SCANCODE_S]) {
		offsetMove(2);
		pathfinder = nullptr;
	} else if (key_down[SDL_SCANCODE_A]) {
		offsetMove(1);
		pathfinder = nullptr;
	} else if (key_down[SDL_SCANCODE_D]) {
		offsetMove(3);
		pathfinder = nullptr;
	}

	///// Setting up camera

    float angleDiff = angleFacing * M_PI / 2.f - m_camera.yaw;
    if (angleDiff > M_PI) {
        angleDiff -= M_PI * 2.f;
    }
    if (angleDiff < -M_PI) {
        angleDiff += M_PI * 2.f;
    }
    m_camera.yaw += angleDiff * cameraTurnSpeed;
    if (m_camera.yaw < 0.f) {
		m_camera.yaw += M_PI * 2.f;
    }
    if (m_camera.yaw >= M_PI * 2.f) {
		m_camera.yaw -= M_PI * 2.f;
    }

	float cameraToY = cameraHeight;

	if (!falling) {
		tile *tile_from = m_maze.getTile(tx_prev, ty_prev);
		tile *tile_to = m_maze.getTile(tx, ty);

		if (tile_from->state == STATE_ITEM || tile_to->state == STATE_ITEM) {
			mazeItem *item_from = m_maze.findItem(tile_from);
			mazeItem *item_to = m_maze.findItem(tile_to);

			if (!(lastMoveAngle % 2)
				&& ((item_to && item_to->type == ITEM_BRIDGE)
					|| (item_from && item_from->type == ITEM_BRIDGE && moving >= ticksPerMove / 2))) {
				cameraToY = bridgeArcHeight * 0.4f;
			} else if ((item_to && item_to->type == ITEM_BRIDGE) || (item_from && item_from->type == ITEM_BRIDGE)) {
				cameraToY += marblePos.y - marbleRadius;
			}
		}
	}

	float pitch = glm::radians(cameraPitch);
	float radius = cosf(pitch) * cameraDistance;

	cameraTargetPos.y += (cameraToY - cameraTargetPos.y) * cameraSpeed;

	if (falling) {
		float cameraToPitch = atan2f(m_camera.position.y - marblePos.y, radius);
		float delta = cameraToPitch - m_camera.pitch;
		m_camera.pitch += delta * cameraSpeed;
	} else {
		m_camera.pitch = pitch;
	}
}

void game::updateMatrices(float deltaNano) {
	static glm::vec3 lastPos = marblePos;

	if (moving > 0) {
		float moveAmt = deltaNano * (float) options.tickrate / (float) ticksPerMove / 1000000000.f;
		if (moveAmt >= 1.f) {
			startX = marblePos.x = (tx + 0.5f) * tileSize;
			startZ = marblePos.z = (ty + 0.5f) * tileSize;
		} else {
			marblePos.x += ((tx + 0.5f) * tileSize - startX) * moveAmt;
			marblePos.z += ((ty + 0.5f) * tileSize - startZ) * moveAmt;
		}
	}

	rollMarble(marblePos - lastPos);

    setupMarble(deltaNano);

	cameraTargetPos.x = marblePos.x;
	cameraTargetPos.z = marblePos.z;

	float radius = cosf(m_camera.pitch) * cameraDistance;
	float distX = sinf(m_camera.yaw) * radius;
	float distY = sinf(m_camera.pitch) * cameraDistance;
	float distZ = cosf(m_camera.yaw) * radius;

	m_camera.position = cameraTargetPos + glm::vec3(distX, distY, distZ);

	glm::mat4 m_position = glm::translate(glm::mat4(1.f), -m_camera.position);
	glm::mat4 m_pitch = glm::rotate(glm::mat4(1.f), m_camera.pitch, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 m_yaw = glm::rotate(glm::mat4(1.f), -m_camera.yaw, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 m_roll = glm::rotate(glm::mat4(1.f), -m_camera.roll, glm::vec3(0.f, 0.f, 1.f));

	m_view = m_roll * m_pitch * m_yaw * m_position;

	lastPos = marblePos;
}

void game::handleEvent(SDL_Event &e) {
	switch (e.type) {
	case SDL_KEYDOWN:
		if (!e.key.repeat) switch (e.key.keysym.scancode) {
		case SDL_SCANCODE_P:
			startPathing();
			break;
		case SDL_SCANCODE_R:
			pathfinder = nullptr;
			teleportToStart(true);
			break;
		case SDL_SCANCODE_M:
			loadMusic();
			break;
		case SDL_SCANCODE_LEFT:
			angleFacing = (angleFacing + 1) % 4;
			break;
		case SDL_SCANCODE_RIGHT:
			angleFacing = (angleFacing + 3) % 4;
			break;
		case SDL_SCANCODE_SPACE:
			useItem();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
