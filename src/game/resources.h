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

#define BINARY_START(name) _binary_##name##_start
#define BINARY_END(name) _binary_##name##_end
#define BINARY_SIZE(name) (BINARY_END(name) - BINARY_START(name))
#define BINARY_RESOURCE(name) std::string(BINARY_START(name), BINARY_SIZE(name))

#define DECLARE_BINARY(name) extern char BINARY_START(name)[]; extern char BINARY_END(name)[];

#define DECLARE_BINARY_EXTERN(name) extern resource BINARY_RESOURCE(name);
#define GET_RESOURCE(name) BINARY_RESOURCE(name)

#define SHADER_NAME(name) src_game_shader_##name##_glsl
#define DECLARE_SHADER(name) DECLARE_BINARY(SHADER_NAME(name))
#define SHADER_RESOURCE(name) BINARY_RESOURCE(SHADER_NAME(name))

DECLARE_BINARY(resource_materials_txt)
DECLARE_BINARY(resource_options_txt)

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
