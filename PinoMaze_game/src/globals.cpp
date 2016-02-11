#include "globals.h"

#include <cstdio>

#include <GL/glew.h>
#include <GL/GL.h>

bool checkGlError(const char *message) {
	GLenum error = glGetError();
	if (error == GL_NO_ERROR) {
		return true;
	} else {
		fprintf(stderr, "%s: %s\n", message, glewGetErrorString(error));
		return false;
	}
}

const int windowWidth = 800;
const int windowHeight = 600;
const bool fullscreen = false;
const int tickrate = 60;

const float tileSize = 1.f;
const float blockHeight = 0.15f;

const float startBoxSize = 0.8f;
const float startBoxHeight = 0.01f;

const float marbleRadius = 0.07f;

const float pillarHeight = 0.54f;
const float pillarSize = 0.13f;

const float wallHeight = 0.5f;
const float wallThickness = 0.1f;

const float bridgeArcHeight = 0.4f;
const float bridgeArcThickness = 0.027f;
const float bridgeWallHeight = 0.65f;

const int bridgeSubsH = 10;
const int bridgeSubsV = 6;

const float skyboxSize = 20.f;

const int ticksPerMove = 25;
const int fallingDelay = 40;

const float gravityAccel = 0.0012f;

const float cameraHeight = 0.26f;
const float cameraDistance = 0.33f;
const float cameraPitch = 15.f;
const float cameraSpeed = 0.1f;
const float cameraTurnSpeed = 0.12f;

const float shadowArea = 4.f;

const float musicVolume = 0.6f;

const bool enableTextures = true;
const bool enableNormalMaps = true;