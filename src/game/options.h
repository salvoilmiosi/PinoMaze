#ifndef __OPTIONS_H__
#define __OPTIONS_H__

constexpr float tileSize = 1.f;
constexpr float blockHeight = 0.07f;

constexpr float startBoxSize = 0.8f;
constexpr float startBoxHeight = 0.01f;

constexpr float teleportRadius1 = 0.46f;
constexpr float teleportRadius2 = 0.33f;
constexpr float teleportHeight = 0.06f;

constexpr float marbleRadius = 0.07f;

constexpr float pillarHeight = 0.54f;
constexpr float pillarSize = 0.13f;

constexpr float wallHeight = 0.5f;
constexpr float wallThickness = 0.1f;
constexpr size_t numWallMaterials = 3;

constexpr float bridgeArcHeight = 0.4f;
constexpr float bridgeArcThickness = 0.027f;
constexpr float bridgeWallHeight = 0.65f;

constexpr int bridgeSubsH = 10;
constexpr int bridgeSubsV = 6;

constexpr int circleSubs = 16;

constexpr float skyboxSize = 20.f;

constexpr int ticksPerMove = 25;
constexpr int fallingDelay = 40;
constexpr int teleportTicks = 72;

constexpr float gravityAccel = 9.8f;

constexpr float cameraHeight = 0.26f;
constexpr float cameraDistance = 0.33f;
constexpr float cameraPitch = 15.f;
constexpr float cameraSpeed = 0.05f;
constexpr float cameraTurnSpeed = 0.05f;

constexpr float shadowArea = 4.f;

constexpr float musicVolume = 0.2f;

#endif // __OPTIONS_H__