#include "sphere.h"

#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

sphere::sphere(float r, int sh, int sv) :
    sphereRadius(r),
    sphereSubsH(sh),
    sphereSubsV(sv)
{
    std::vector<vertex> vertices;
    std::vector<normal_and_tangent> normal_tangents;
    std::vector<GLuint> indices;

    float angleX, angleY, radius;
    vertex v;
    normal_and_tangent nt;
    for (int i=0; i <= sphereSubsV; ++i) {
        v.texcoords.y = 1.f - (float) i / sphereSubsV;
        angleX = (0.5f - v.texcoords.y) * M_PI;

        radius = cosf(angleX);

		nt.normal.y = sinf(angleX);

        v.position.y = nt.normal.y * sphereRadius;

        for (int j=0; j<=sphereSubsH; ++j) {
            v.texcoords.x = (float) j / sphereSubsH;
            angleY = v.texcoords.x * M_PI * 2.f;

			nt.tangent.x = sinf(angleY);
			nt.tangent.z = cosf(angleY);

			nt.normal.x = nt.tangent.z * radius;
			nt.normal.z = -nt.tangent.x * radius;

            v.position.x = nt.normal.x * sphereRadius;
            v.position.z = nt.normal.z * sphereRadius;

            vertices.push_back(v);
            normal_tangents.push_back(nt);
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

    vao.update_buffer(0, vertices.data(), vertices.size() * sizeof(vertex), {{0, ATTR_VEC3}, {1, ATTR_VEC2}});
    vao.update_buffer(1, normal_tangents.data(), normal_tangents.size() * sizeof(normal_and_tangent), {{2, ATTR_VEC3}, {3, ATTR_VEC3}});
    vao.update_indices(indices.data(), indices.size());
}