#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include "resource_load.h"

#include <string>
struct resource : public std::string {
    const char *name;

    resource(const char *name, const char *data, const size_t size) : std::string(data, size), name(name) {}
};

#define BINARY_START(name) _binary_##name##_start
#define BINARY_END(name) _binary_##name##_end
#define BINARY_SIZE(name) (BINARY_END(name) - BINARY_START(name))
#define BINARY_RESOURCE(name) resource(#name, BINARY_START(name), BINARY_SIZE(name))

#define DECLARE_BINARY(name) extern char BINARY_START(name)[]; extern char BINARY_END(name)[];

#define DECLARE_BINARY_EXTERN(name) extern resource BINARY_RESOURCE(name);
#define GET_RESOURCE(name) BINARY_RESOURCE(name)

#define SHADER_NAME(name) src_game_shader_##name##_glsl
#define DECLARE_SHADER(name) DECLARE_BINARY(SHADER_NAME(name))
#define SHADER_RESOURCE(name) BINARY_RESOURCE(SHADER_NAME(name))

#endif // __RESOURCES_H__