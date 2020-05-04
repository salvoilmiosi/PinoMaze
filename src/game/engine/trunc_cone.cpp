#include "trunc_cone.h"

#include <vector>
#include <cmath>

static void addCircle(std::vector<base_vertex> &vertices, float radius, float tex_radius, float h, int divs) {
    base_vertex vertex;

    vertex.position = glm::vec3(0.f, h, 0.f);
    vertex.texcoords = glm::vec2(0.5f, 0.5f);

    vertices.push_back(vertex);

    for (int i=0; i<divs; ++i) {
        float theta = (float) i / divs * 2.f * M_PI;
        vertex.position = glm::vec3(cosf(theta) * radius, h, sinf(theta) * radius);
        vertex.texcoords = glm::vec2(0.5f + cosf(theta) * tex_radius, 0.5f + sinf(theta) * tex_radius);
        vertices.push_back(vertex);
    }
}

static void addIndices(std::vector<unsigned int> &indices, int divs) {
    for (int i=1; i<=divs; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i % divs + 1);
    }
    for (int i=1; i<=divs; ++i) {
        indices.push_back(divs + 1);
        indices.push_back(i % divs + divs + 2);
        indices.push_back(divs + i + 1);
    }
    for (int i=1; i<=divs; ++i) {
        indices.push_back(i);
        indices.push_back(i + divs + 1);
        indices.push_back(i % divs + 1);

        indices.push_back(i + divs + 1);
        indices.push_back(i % divs + divs + 2);
        indices.push_back(i % divs + 1);
    }
}

trunc_cone::trunc_cone(float r1, float r2, float h, int divs) :
    r1(r1), r2(r2), h(h), divs(divs)
{
    std::vector<base_vertex> vertices;
    std::vector<unsigned int> indices;

    addCircle(vertices, r1, 0.5f, 0.f, divs);
    addCircle(vertices, r2, 0.5f * r2 / r1, h, divs);
    addIndices(indices, divs);

    calculate_buffers(vertices.data(), vertices.size(), indices.data(), indices.size());
}