#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>

#define BINARY_START(name) _binary_##name##_start
#define BINARY_END(name) _binary_##name##_end
#define BINARY_SIZE(name) (BINARY_END(name) - BINARY_START(name))

#define BINARY_DECLARE(name) extern char BINARY_START(name)[]; extern char BINARY_END(name)[];
#define BINARY_STRING(name) std::string(BINARY_START(name), BINARY_SIZE(name))

#define SHADER_NAME(name) src_shader_##name##_glsl
#define DECLARE_SHADER(name) BINARY_DECLARE(SHADER_NAME(name))
#define SHADER_STRING(name) BINARY_STRING(SHADER_NAME(name))

DECLARE_SHADER(vertex)
DECLARE_SHADER(fragment)

#endif