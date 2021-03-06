#include "base_model.h"

void base_model::calculate_buffers(const base_vertex *vertices, const size_t vertex_count, const GLuint *indices, const size_t index_count) {
    normal_and_tangent data[vertex_count];

    for (size_t i=0; i < index_count; i+=3) {
        const base_vertex &v0 = vertices[indices[i]];
        const base_vertex &v1 = vertices[indices[i+1]];
        const base_vertex &v2 = vertices[indices[i+2]];

        glm::vec3 deltaPos1(v1.position.x-v0.position.x, v1.position.y-v0.position.y, v1.position.z-v0.position.z);
        glm::vec3 deltaPos2(v2.position.x-v0.position.x, v2.position.y-v0.position.y, v2.position.z-v0.position.z);

        glm::vec2 deltaUV1(v1.texcoords.x-v0.texcoords.x, v1.texcoords.y-v0.texcoords.y);
        glm::vec2 deltaUV2(v2.texcoords.x-v0.texcoords.x, v2.texcoords.y-v0.texcoords.y);

        glm::vec3 normal = glm::normalize(glm::cross(deltaPos1, deltaPos2));
        glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x));

        data[indices[i]].normal = normal;
        data[indices[i+1]].normal = normal;
        data[indices[i+2]].normal = normal;

        data[indices[i]].tangent = tangent;
        data[indices[i+1]].tangent = tangent;
        data[indices[i+2]].tangent = tangent;
    }

    update_vertices(0, vertices, vertex_count, {{0, ATTR_VEC3}, {1, ATTR_VEC2}});
    update_vertices(1, data, vertex_count, {{2, ATTR_VEC3}, {3, ATTR_VEC3}});
    update_indices(indices, index_count);
}