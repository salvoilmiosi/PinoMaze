#include "shader.h"

#include <iostream>

static const GLenum gl_shader_types[] = {
	GL_VERTEX_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER
};

shader::shader(const char *name, shader_type type, const std::string &source) {
	gl_shaderid = glCreateShader(gl_shader_types[type]);
    const GLchar *source_cstr = source.c_str();
    const GLsizei source_size = source.size(); 

	glShaderSource(gl_shaderid, 1, &source_cstr, &source_size);
	glCompileShader(gl_shaderid);

	int length = 0;
	int max_length = 0;

	glGetShaderiv(gl_shaderid, GL_INFO_LOG_LENGTH, &max_length);
	std::string info_log(max_length, '\0');
	glGetShaderInfoLog(gl_shaderid, max_length, &length, info_log.data());

	if (length > 0) {
		std::cout << "In shader " << name << ":" << std::endl << info_log << std::endl;
	}

	GLint compiled = GL_FALSE;
	glGetShaderiv(gl_shaderid, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		throw std::string("Failed to compile shader ") + name + "\n" + info_log;
	}
}

shader::~shader() {
	glDeleteShader(gl_shaderid);
}

shader_program::shader_program(const char *name) : name(name) {
	gl_programid = glCreateProgram();
}

shader_program::~shader_program() {
	glDeleteProgram(gl_programid);
}

void shader_program::use() {
	glUseProgram(gl_programid);
	update_uniforms();
}

void shader_program::link() {
	glLinkProgram(gl_programid);

	int length = 0;
	int max_length = 0;

	glGetProgramiv(gl_programid, GL_INFO_LOG_LENGTH, &max_length);
	std::string info_log(max_length, '\0');
	glGetProgramInfoLog(gl_programid, max_length, &length, info_log.data());

	if (length > 0) {
		std::cout << "In program " << name << ":" << std::endl << info_log << std::endl;
	}

	GLint linked = GL_FALSE;
	glGetProgramiv(gl_programid, GL_LINK_STATUS, &linked);

	if (linked == GL_FALSE) {
		throw std::string("Failed to link program ") + name + "\n" + info_log;
	}
}

template<> inline void shader_program::uniform<bool>::update() {
	glUniform1i(location, *data);
}
template<> inline void shader_program::uniform<int>::update() {
	glUniform1i(location, *data);
}
template<> inline void shader_program::uniform<float>::update() {
	glUniform1f(location, *data);
}
template<> inline void shader_program::uniform<glm::vec2>::update() {
	glUniform2fv(location, 1, glm::value_ptr(*data));
}
template<> inline void shader_program::uniform<glm::vec3>::update() {
	glUniform3fv(location, 1, glm::value_ptr(*data));
}
template<> inline void shader_program::uniform<glm::vec4>::update() {
	glUniform4fv(location, 1, glm::value_ptr(*data));
}
template<> inline void shader_program::uniform<glm::mat4>::update() {
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(*data));
}

void shader_program::update_uniforms() {
	mpl::for_each_in_tuple(p_uniforms, [&](auto &uni_vector) {
		for (auto &uni : uni_vector) {
			uni.update();
		}
	});
}