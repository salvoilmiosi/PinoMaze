#ifndef __GLOBALS_H__
#define __GLOBALS_H__

static auto MAX = [](auto a, auto b) {
	return a > b ? a : b;
};

static auto MIN = [](auto a, auto b) {
	return a > b ? b : a;
};

bool checkGlError(const char *message);

extern const int windowWidth;
extern const int windowHeight;
extern const bool fullscreen;
extern const int tickrate;

extern const float tileSize;
extern const float blockHeight;

extern const float startBoxSize;
extern const float startBoxHeight;

extern const float marbleRadius;

extern const float pillarHeight;
extern const float pillarSize;

extern const float wallHeight;
extern const float wallThickness;

extern const float bridgeArcHeight;
extern const float bridgeArcThickness;
extern const float bridgeWallHeight;

extern const int bridgeSubsH;
extern const int bridgeSubsV;

extern const float skyboxSize;

extern const int ticksPerMove;
extern const int fallingDelay;

extern const float gravityAccel;

extern const float cameraHeight;
extern const float cameraDistance;
extern const float cameraPitch;
extern const float cameraSpeed;
extern const float cameraTurnSpeed;

extern const float shadowArea;

extern const float musicVolume;

extern const bool enableTextures;
extern const bool enableNormalMaps;

#endif // __GLOBALS_H__