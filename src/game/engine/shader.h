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

enum shader_type {
	SHADER_VERTEX,
	SHADER_GEOMETRY,
	SHADER_FRAGMENT
};

class shader {
public:
	shader(const char *name, shader_type type, const std::string &source);
	~shader();

public:
	GLuint getId() {
		return gl_shaderid;
	}

private:
	GLuint gl_shaderid = 0;

	friend class shader_program;
};

class shader_program {
protected:
	shader_program();
	~shader_program();

	template<typename ... Ts>
	void link_shaders(const Ts& ... shaders) {
		(glAttachShader(gl_programid, shaders.gl_shaderid) , ...);
		glLinkProgram(gl_programid);
	}

	template<typename ... Ts>
	void detach_shaders(const Ts& ... shaders) {
		(glDetachShader(gl_programid, shaders.gl_shaderid) , ...);
	}

public:
	template<typename T>
	void add_uniform(const char *name, T *data) {
		int location = glGetUniformLocation(gl_programid, name);
		std::get<std::vector<uniform<T>>>(p_uniforms).emplace_back(location, name, data);
	}

	void use();
	void update_uniforms();
	
private:
	GLuint gl_programid;

	template<typename T>
	struct uniform {
		GLint location;
		const char *name;
		T *data;

		uniform(GLint location, const char *name, T *data) : location(location), name(name), data(data) {}

		void update();
	};

	using uniform_types = mpl::TypeList<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;
	template<typename ... Ts>
	using uniform_list = std::tuple<std::vector<uniform<Ts>>...>;

	mpl::Rename<uniform_list, uniform_types> p_uniforms;
};

class vf_shader : public shader_program {
public:
	vf_shader(const char *name, const std::string &vertex_source, const std::string &fragment_source);
	~vf_shader();

private:
	shader m_vertex, m_fragment;
};

#endif