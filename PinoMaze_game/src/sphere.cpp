#include "sphere.h"

void sphere::setSize(float r, int sh, int sv) {
    sphereRadius = r;
    sphereSubsH = sh;
    sphereSubsV = sv;
}

bool sphere::init() {
    vector<glm::vec3> normals;
	vector<glm::vec3> tangents;

    float angleX, angleY, radius;
    vertex v;
    glm::vec3 n;
	glm::vec3 t;
    for (int i=0; i <= sphereSubsV; ++i) {
        v.v = 1.f - (float) i / sphereSubsV;
        angleX = (0.5f - v.v) * M_PI_F;

        radius = cosf(angleX);

		n.y = sinf(angleX);

        v.y = n.y * sphereRadius;

        for (int j=0; j<=sphereSubsH; ++j) {
            v.u = (float) j / sphereSubsH;
            angleY = v.u * M_PI_F * 2.f;

			t.x = sinf(angleY);
			t.z = cosf(angleY);

			n.x = t.z * radius;
			n.z = -t.x * radius;

            v.x = n.x * sphereRadius;
            v.z = n.z * sphereRadius;

            vertices.push_back(v);
            normals.push_back(n);

			tangents.push_back(t);
        }
    }

    for (int i=0; i<sphereSubsV; ++i) {
        for (int j=0; j<sphereSubsH; ++j) {
            GLuint v0 = i * (sphereSubsH+1) + j;
            GLuint v1 = v0 + 1;
            GLuint v2 = (i+1) * (sphereSubsH+1) + j;
            GLuint v3 = v2 + 1;

            if (i != 0) {
                indices.push_back(v0);
                indices.push_back(v1);
                indices.push_back(v2);
            }

            if (i != sphereSubsV - 1) {
                indices.push_back(v2);
                indices.push_back(v1);
                indices.push_back(v3);
            }
        }
    }

    if (!createBuffers(vertices.data(), indices.data(), vertices.size(), indices.size())) return false;
    if (!createMatrixBuffer(matrices.data(), matrices.size())) return false;

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &tangentBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);

    if (!loadToVAO()) return false;

    return true;
}

void sphere::render() {
    if (!matrices.empty()) {
        bindModel();
        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, (GLsizei)matrices.size());
        unbindModel();
    }
}
