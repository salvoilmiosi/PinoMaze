#ifndef __SHADER_PROGRAM_H__
#define __SHADER_PROGRAM_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

class shaderProgram {
protected:
    GLuint programID;
    GLuint vertexShader;
    GLuint fragmentShader;

	const char *programName;

public:
	shaderProgram() : shaderProgram(nullptr) {}

    shaderProgram(const char *name);
    virtual ~shaderProgram();

protected:
	bool loadProgramFromSource(const char *vertexSource, const char *fragmentSource);

    bool loadProgramFromFile(const char *vertexFilename, const char *fragmentFilename);
    bool loadProgramFromResource(int ID_VERTEX, int ID_FRAGMENT);

public:
    virtual bool init() = 0;

    bool bindProgram();
    virtual void unbindProgram();

protected:
    void setUniform(GLint address, const glm::mat4 &m);
    void setUniform(GLint address, const glm::vec3 &v);
    void setUniform(GLint address, const glm::vec2 &v);
    void setUniform(GLint address, const int &i);
    void setUniform(GLint address, const unsigned int &i);
    void setUniform(GLint address, const float &f);
    void setUniform(GLint address, const bool &b);

protected:
    virtual void bindAddresses() = 0;

    void printProgramLog(GLuint program);
    void printShaderLog(GLuint shader, const char *type);
};

#endif // __SHADER_PROGRAM_H__
