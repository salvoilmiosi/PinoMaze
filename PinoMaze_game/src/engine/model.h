#ifndef __MODEL_H__
#define __MODEL_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include <vector>

class model {
protected:
    struct vertex {
        float x, y, z;
        float u, v;
    };

protected:
    GLuint vertexArray = 0;

    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint tangentBuffer = 0;
    GLuint matrixBuffer = 0;

    std::vector<glm::mat4> matrices;

public:
    virtual ~model();

public:
    virtual bool init() = 0;

    virtual void bindModel();
    virtual void unbindModel();
    virtual void render() = 0;

public:
    void addMatrix(const glm::mat4 &matrix);
    void loadMatrix(const glm::mat4 &matrix);
	void updateMatrices();

protected:
    bool createBuffers(const vertex *vertices, const GLuint *indices, size_t vertex_count, size_t index_count);
    bool calculateNormals(const vertex *vertices, const GLuint *indices, size_t vertex_count, size_t index_count);
    bool calculateTangents(const vertex *vertices, const GLuint *indices, size_t vertex_count, size_t index_count);
    bool createMatrixBuffer(const glm::mat4 *matrices, size_t matrix_count = 1);
    bool loadToVAO(bool createNew = false);
};

#endif // __MODEL_H__
