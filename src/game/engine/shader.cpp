#include "shader.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

shader::shader(const char *name, const GLenum type, const std::string &source) {
	gl_shaderid = glCreateShader(type);

    const GLchar *source_cstr = source.c_str();
    const GLsizei source_size = source.size(); 

	glShaderSource(gl_shaderid, 1, &source_cstr, &source_size);
	glCompileShader(gl_shaderid);

	int length = 0;
	int max_length = 0;

	glGetShaderiv(gl_shaderid, GL_INFO_LOG_LENGTH, &max_length);

	char *info_log = (char *) malloc(max_length);

	glGetShaderInfoLog(gl_shaderid, max_length, &length, info_log);
	if (length > 0) {
		std::cout << "Compile error in shader " << name << ":" << std::endl << info_log << std::endl;
	}

	free(info_log);

	glGetShaderiv(gl_shaderid, GL_COMPILE_STATUS, &compiled);
}

shader::~shader() {
    glDeleteShader(gl_shaderid);
}

void shader_program::update_uniforms() {
    for (uniform_location &i : uniforms) {
		switch(i.uni->type) {
		case uniform::TYPE_INT:
			glUniform1i(i.location, i.uni->value_int);
			break;
		case uniform::TYPE_FLOAT:
			glUniform1f(i.location, i.uni->value_float);
			break;
		case uniform::TYPE_VEC2:
			glUniform2fv(i.location, 1, glm::value_ptr(i.uni->value_vec2));
			break;
        case uniform::TYPE_VEC3:
            glUniform3fv(i.location, 1, glm::value_ptr(i.uni->value_vec3));
            break;
        case uniform::TYPE_MAT4:
            glUniformMatrix4fv(i.location, 1, false, glm::value_ptr(i.uni->value_mat4));
            break;
		}
	}
}
