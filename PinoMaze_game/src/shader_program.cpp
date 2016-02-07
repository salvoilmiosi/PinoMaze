#include "shader_program.h"

#include "res_loader.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

shaderProgram::shaderProgram(const char *name) : programName(name) {
	programID = 0;
	vertexShader = 0;
	fragmentShader = 0;
}

shaderProgram::~shaderProgram() {
	clean();
}

bool shaderProgram::loadProgramFromSource(const char *vertexSource, const char *fragmentSource) {
	assert(vertexSource != nullptr);
	assert(fragmentSource != nullptr);

	programID = glCreateProgram();
	GLint compiled = 0;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printShaderLog(vertexShader, "vertex");
		return false;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printShaderLog(fragmentShader, "fragment");
		return false;
	}

	glBindFragDataLocation(programID, 0, "color");

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);

	glLinkProgram(programID);
	bindAddresses();
	return true;
}

bool shaderProgram::loadProgramFromFile(const char *vertexFilename, const char *fragmentFilename) {
	std::string vertexSource = loadStringFromFile(vertexFilename);
	if (vertexSource.empty()) return false;

	std::string fragmentSource = loadStringFromFile(fragmentFilename);
	if (fragmentSource.empty()) return false;

	return loadProgramFromSource(vertexSource.c_str(), fragmentSource.c_str());
}

bool shaderProgram::loadProgramFromResource(int VERTEX_ID, int FRAGMENT_ID) {
	std::string vertexSource = loadStringFromResource(VERTEX_ID);
	if (vertexSource.empty()) return false;

	std::string fragmentSource = loadStringFromResource(FRAGMENT_ID);
	if (fragmentSource.empty()) return false;

	return loadProgramFromSource(vertexSource.c_str(), fragmentSource.c_str());
}

void shaderProgram::clean() {
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(programID);
}

bool shaderProgram::bindProgram() {
	glUseProgram(programID);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		printProgramLog(programID);
		return false;
	}
	return true;
}

void shaderProgram::unbindProgram() {
	glUseProgram(0);
}

void shaderProgram::setUniform(GLint address, const glm::mat4 &m) {
	glUniformMatrix4fv(address, 1, false, glm::value_ptr(m));
}

void shaderProgram::setUniform(GLint address, const glm::vec3 &v) {
	glUniform3fv(address, 1, glm::value_ptr(v));
}

void shaderProgram::setUniform(GLint address, const glm::vec2 &v) {
	glUniform2fv(address, 1, glm::value_ptr(v));
}

void shaderProgram::setUniform(GLint address, const int &i) {
	glUniform1i(address, i);
}

void shaderProgram::setUniform(GLint address, const unsigned int &i) {
	glUniform1i(address, i);
}

void shaderProgram::setUniform(GLint address, const float &f) {
	glUniform1f(address, f);
}

void shaderProgram::setUniform(GLint address, const bool &b) {
	glUniform1i(address, b);
}

void shaderProgram::printProgramLog(GLuint program) {
	if (glIsProgram(program)) {
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = new char[maxLength];
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) {
			printf("Error in program \"%s\":\n%s", programName, infoLog);
		}

		delete[] infoLog;
	} else {
		printf("Name %d is not a program\n", program);
	}
}

void shaderProgram::printShaderLog(GLuint shader, const char *type) {
	if (glIsShader(shader)) {
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = new char[maxLength];
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) {
			printf("Error in %s shader \"%s\":\n%s", type, programName, infoLog);
		}

		delete[] infoLog;
	} else {
		printf("Name %d is not a shader\n", shader);
	}
}
