#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <tuple>

#include "mpl.h"
#include "texture.h"

template<typename T>
struct uniform {
	GLint location;
	const char *name;
	T *data;

	uniform(GLint location, const char *name, T *data) : location(location), name(name), data(data) {}
};

using uniform_types = mpl::TypeList<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

class shader {
private:
	const char *name;

	GLuint gl_programid = 0;
	GLuint gl_vertexid = 0;
	GLuint gl_fragmentid = 0;

	template<typename ... Ts>
	using uniform_list = std::tuple<std::vector<uniform<Ts>>...>;

	mpl::Rename<uniform_list, uniform_types> p_uniforms;

public:
	shader(const char *name, const std::string &vertex_src, const std::string &fragment_src);
	~shader();

public:
	template<typename T>
	void add_uniform(const char *name, T *data) {
		int location = glGetUniformLocation(gl_programid, name);
		std::get<std::vector<uniform<T>>>(p_uniforms).emplace_back(location, name, data);
	}

	void use_program();

	void update_uniforms();

private:
	void compile(GLuint gl_shaderid, const std::string &source);
};

#endif