#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <tuple>

#include "mpl.h"

template<typename T>
struct uniform {
	GLint location;
	const char *name;
	T *data;

	uniform(GLint location, const char *name, T *data) : location(location), name(name), data(data) {}
};

inline void updateValue(const uniform<int> &uni) {
	glUniform1i(uni.location, *uni.data);
}
inline void updateValue(const uniform<float> &uni) {
	glUniform1f(uni.location, *uni.data);
}
inline void updateValue(const uniform<glm::vec2> &uni) {
	glUniform2fv(uni.location, 1, glm::value_ptr(*uni.data));
}
inline void updateValue(const uniform<glm::vec3> &uni) {
	glUniform3fv(uni.location, 1, glm::value_ptr(*uni.data));
}
inline void updateValue(const uniform<glm::vec4> &uni) {
	glUniform4fv(uni.location, 1, glm::value_ptr(*uni.data));
}
inline void updateValue(const uniform<glm::mat4> &uni) {
	glUniformMatrix4fv(uni.location, 1, false, glm::value_ptr(*uni.data));
}

using uniform_types = mpl::TypeList<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

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