#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <SDL2/SDL_image.h>
#include <string>

#include "resource_load.h"

#include "engine/sound.h"

bool openResourceFile(const char *dir, const char *filename);

template<typename ... T>
inline bool openResourceFiles(const char *dir, T ... filenames) {
	return (openResourceFile(dir, filenames) && ...);
}

SDL_Surface *loadImageFromResource(const char *RES_ID);
bool loadWaveFromResource(sound &snd, const char *RES_ID);
bool loadMusicFromResource(music &mus, const char *RES_ID);

#define RESOURCE_DATA(name) __resource__##name
#define RESOURCE_LENGTH(name) __resource__##name##_length
#define BINARY_RESOURCE(name) std::string(RESOURCE_DATA(name), RESOURCE_DATA(name) + RESOURCE_LENGTH(name))

#define DECLARE_RESOURCE(name) extern char RESOURCE_DATA(name)[]; extern unsigned long long RESOURCE_LENGTH(name);

#define SHADER_NAME(name) name##_glsl
#define DECLARE_SHADER(name) DECLARE_RESOURCE(SHADER_NAME(name))
#define SHADER_RESOURCE(name) BINARY_RESOURCE(SHADER_NAME(name))

DECLARE_RESOURCE(materials_txt)

DECLARE_SHADER(s_billboard_v)
DECLARE_SHADER(s_billboard_g)
DECLARE_SHADER(s_billboard_f)
DECLARE_SHADER(s_hud_v)
DECLARE_SHADER(s_hud_f)
DECLARE_SHADER(s_particle_v)
DECLARE_SHADER(s_particle_g)
DECLARE_SHADER(s_shadow_v)
DECLARE_SHADER(s_shadow_f)
DECLARE_SHADER(s_skybox_v)
DECLARE_SHADER(s_skybox_f)
DECLARE_SHADER(s_water_v)
DECLARE_SHADER(s_water_f)
DECLARE_SHADER(s_world_v)
DECLARE_SHADER(s_world_f)

#endif // __RESOURCES_H__
