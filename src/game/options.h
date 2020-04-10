#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <cstdio>
#include <GL/glew.h>
#include <GL/gl.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

static auto MAX = [](auto a, auto b) {
	return a > b ? a : b;
};

static auto MIN = [](auto a, auto b) {
	return a > b ? b : a;
};

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;
constexpr bool fullscreen = false;
constexpr int tickrate = 60;

constexpr float tileSize = 1.f;
constexpr float blockHeight = 0.15f;

constexpr float startBoxSize = 0.8f;
constexpr float startBoxHeight = 0.01f;

constexpr float marbleRadius = 0.07f;

constexpr float pillarHeight = 0.54f;
constexpr float pillarSize = 0.13f;

constexpr float wallHeight = 0.5f;
constexpr float wallThickness = 0.1f;

constexpr float bridgeArcHeight = 0.4f;
constexpr float bridgeArcThickness = 0.027f;
constexpr float bridgeWallHeight = 0.65f;

constexpr int bridgeSubsH = 10;
constexpr int bridgeSubsV = 6;

constexpr float skyboxSize = 20.f;

constexpr int ticksPerMove = 25;
constexpr int fallingDelay = 40;

constexpr float gravityAccel = 0.0012f;

constexpr float cameraHeight = 0.26f;
constexpr float cameraDistance = 0.33f;
constexpr float cameraPitch = 15.f;
constexpr float cameraSpeed = 0.1f;
constexpr float cameraTurnSpeed = 0.12f;

constexpr float shadowArea = 4.f;

constexpr float musicVolume = 0.6f;

constexpr bool enableTextures = true;
constexpr bool enableNormalMaps = true;

#endif // __OPTIONS_H__