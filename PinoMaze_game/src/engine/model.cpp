#include "model.h"

#include <cstdio>
#include <vector>

#include "../globals.h"

using namespace std;

model::~model() {
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &tangentBuffer);
	glDeleteBuffers(1, &matrixBuffer);
	glDeleteVertexArrays(1, &vertexArray);
}

void model::bindModel() {
    glBindVertexArray(vertexArray);
}

void model::unbindModel() {
    glBindVertexArray(0);
}

void model::addMatrix(const glm::mat4 &matrix) {
    matrices.push_back(matrix);
}

void model::loadMatrix(const glm::mat4 &matrix) {
	matrices.clear();
	matrices.push_back(matrix);
	updateMatrices();
}

void model::updateMatrices() {
	glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * matrices.size(), matrices.data());
}

bool model::loadToVAO(bool createNew) {
    if (vertexArray == 0 || createNew) {
        glGenVertexArrays(1, &vertexArray);
    }
    glBindVertexArray(vertexArray);

    if (vertexBuffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertex), (void*)(3*sizeof(float)));
    }
	if (indexBuffer != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	}
    if (normalBuffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
    }
    if (tangentBuffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
		glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, 0);
    }
    if (matrixBuffer != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

        glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(glm::mat4), 0);
        glVertexAttribPointer(5, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(4*sizeof(float)));
        glVertexAttribPointer(6, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(8*sizeof(float)));
        glVertexAttribPointer(7, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(12*sizeof(float)));
    }
    glBindVertexArray(0);

	return checkGlError("Could not init model");
}

bool model::createBuffers(const vertex *vertices, const GLuint *indices,
	size_t vertex_count, size_t index_count) {

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(vertex), vertices, GL_STATIC_DRAW);

	if (!checkGlError("Failed to create vertex buffer")) return false;

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

	if (!checkGlError("Failed to create index buffer")) return false;

    return true;
}

bool model::calculateNormals(const vertex *vertices, const GLuint *indices,
	size_t vertex_count, size_t index_count) {

    std::vector<glm::vec3> data(vertex_count);

    for (size_t i=0; i < index_count; i+=3) {
        const vertex &v0 = vertices[indices[i]];
        const vertex &v1 = vertices[indices[i+1]];
        const vertex &v2 = vertices[indices[i+2]];

        glm::vec3 deltaPos1(v1.x-v0.x, v1.y-v0.y, v1.z-v0.z);
        glm::vec3 deltaPos2(v2.x-v0.x, v2.y-v0.y, v2.z-v0.z);

        glm::vec3 normal = glm::normalize(glm::cross(deltaPos1, deltaPos2));

        data[indices[i]] += normal;
        data[indices[i+1]] += normal;
        data[indices[i+2]] += normal;
    }

    for (size_t i=0; i < vertex_count; ++i) {
        glm::vec3 &normal = data[i];
        normal = glm::normalize(normal);
    }

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, data.data(), GL_STATIC_DRAW);

	return checkGlError("Failed to create normal buffer");
}

bool model::calculateTangents(const vertex *vertices, const GLuint *indices,
	size_t vertex_count, size_t index_count) {

	std::vector<glm::vec3> data(vertex_count);

    for (size_t i=0; i < index_count; i+=3) {
        const vertex &v0 = vertices[indices[i]];
        const vertex &v1 = vertices[indices[i+1]];
        const vertex &v2 = vertices[indices[i+2]];

        glm::vec3 deltaPos1(v1.x-v0.x, v1.y-v0.y, v1.z-v0.z);
        glm::vec3 deltaPos2(v2.x-v0.x, v2.y-v0.y, v2.z-v0.z);

        glm::vec2 deltaUV1(v1.u-v0.u, v1.v-v0.v);
        glm::vec2 deltaUV2(v2.u-v0.u, v2.v-v0.v);

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        data[indices[i]] = tangent;
        data[indices[i+1]] = tangent;
        data[indices[i+2]] = tangent;
    }

    glGenBuffers(1, &tangentBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, data.data(), GL_STATIC_DRAW);

	return checkGlError("Failed to create tangent buffer");
}

bool model::createMatrixBuffer(const glm::mat4 *matrices, size_t matrix_count) {
    glGenBuffers(1, &matrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
	size_t numMatrices = MAX(1, (int)matrix_count);
    if (matrices && matrix_count > 0) {
        glBufferData(GL_ARRAY_BUFFER, numMatrices * sizeof(glm::mat4), matrices, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numMatrices, nullptr, GL_DYNAMIC_DRAW);
    }

	return checkGlError("Failed to create matrix buffer");
}
