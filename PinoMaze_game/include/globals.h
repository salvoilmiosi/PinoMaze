#ifndef __GLOBALS_H__
#define __GLOBALS_H__

template <typename T> const T MAX(const T& a, const T& b) {
	return a > b ? a : b;
}

template <typename T> const T MIN(const T& a, const T& b) {
	return a > b ? b : a;
}

static constexpr int windowWidth = 800;
static constexpr int windowHeight = 600;
static constexpr int tickrate = 60;

static constexpr float tileSize = 1.f;
static constexpr float blockHeight = 0.15f;

static constexpr float startBoxSize = 0.8f;
static constexpr float startBoxHeight = 0.01f;

static constexpr float marbleRadius = 0.07f;

static constexpr float pillarHeight = 0.54f;
static constexpr float pillarSize = 0.13f;

static constexpr float wallHeight = 0.5f;
static constexpr float wallThickness = 0.1f;

static constexpr float bridgeArcHeight = 0.4f;
static constexpr float bridgeArcThickness = 0.027f;
static constexpr float bridgeWallHeight = 0.63f;

static constexpr int bridgeSubsH = 10;
static constexpr int bridgeSubsV = 3;

static constexpr float skyboxSize = 20.f;

static constexpr int ticksPerMove = 25;
static constexpr int fallingDelay = 40;

static constexpr float gravityAccel = 0.0012f;

static constexpr float cameraHeight = 0.26f;
static constexpr float cameraDistance = 0.33f;
static constexpr float cameraPitch = 15.f;
static constexpr float cameraSpeed = 0.1f;
static constexpr float cameraTurnSpeed = 0.12f;

static constexpr float shadowArea = 4.f;

#endif // __GLOBALS_H__