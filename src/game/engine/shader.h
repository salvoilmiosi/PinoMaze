#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

class shader {
private:
	GLuint gl_shaderid = 0;
	
	const char *name;
	const GLenum type;

	GLint compiled = GL_FALSE;

public:
	shader(const char *name, const GLenum type, const std::string &source);

	~shader();

	bool compile_status() {
		return compiled;
	}

private:
	friend class shader_program;
};

struct uniform {
	const char *name;
	enum {
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_MAT4
	} type;

	union {
		int value_int;
		float value_float;
		glm::vec2 value_vec2;
		glm::vec3 value_vec3;
		glm::mat4 value_mat4;
	};
};

class shader_program {
private:
	GLuint gl_programid = 0;

	struct uniform_location {
		int location;
		uniform *uni;

		uniform_location(int location, uniform *uni) : location(location), uni(uni) {}
	};

	std::vector<shader *> shaders;
	std::vector<uniform_location> uniforms;

public:
	template <typename ... Ts>
	shader_program(Ts &... shaders) : shaders {&shaders ...} {
		gl_programid = glCreateProgram();

		for (shader *i : shaders) {
			glAttachShader(gl_programid, i->gl_shaderid);
		}

		glLinkProgram(gl_programid);
	}

	~shader_program() {
		if (gl_programid) {
			for (shader *i : shaders) {
				glDetachShader(gl_programid, i->gl_shaderid);
			}
			glDeleteProgram(gl_programid);
		}
	}

	void add_uniform(uniform &uni) {
		int location = glGetUniformLocation(gl_programid, uni.name);
		uniforms.emplace_back(location, &uni);
	}

	void add_uniforms() {}

	template <typename ... Ts>
	void add_uniforms(uniform &first, Ts &...unis) {
		add_uniform(first);
		add_uniforms(unis...);
	}

	void use_program() {
		glUseProgram(gl_programid);
		update_uniforms();
	}

	void update_uniforms();
};

#endif