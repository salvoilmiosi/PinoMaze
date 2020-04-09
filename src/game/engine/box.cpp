#include "box.h"

box::box(float w, float h, float l, float uv) :
    width(w), height(h), length(l), texSize(uv)
{
    const vertex vertices[] = {
        // Front face
        {{-width / 2.f, height / 2.f, length / 2.f}, {0.f,             0.f}},
        {{ width / 2.f, height / 2.f, length / 2.f}, {width / texSize, 0.f}},
        {{-width / 2.f,-height / 2.f, length / 2.f}, {0.f,             height / texSize}},
        {{ width / 2.f,-height / 2.f, length / 2.f}, {width / texSize, height / texSize}},

        // Back face
        {{ width / 2.f, height / 2.f,-length / 2.f}, {0.f,             0.f}},
        {{-width / 2.f, height / 2.f,-length / 2.f}, {width / texSize, 0.f}},
        {{ width / 2.f,-height / 2.f,-length / 2.f}, {0.f,             height / texSize}},
        {{-width / 2.f,-height / 2.f,-length / 2.f}, {width / texSize, height / texSize}},

        // Right face
        {{ width / 2.f, height / 2.f, length / 2.f}, {0.f,              0.f}},
        {{ width / 2.f, height / 2.f,-length / 2.f}, {length / texSize, 0.f}},
        {{ width / 2.f,-height / 2.f, length / 2.f}, {0.f,              height / texSize}},
        {{ width / 2.f,-height / 2.f,-length / 2.f}, {length / texSize, height / texSize}},

        // Left face
        {{-width / 2.f, height / 2.f,-length / 2.f}, {0.f,              0.f}},
        {{-width / 2.f, height / 2.f, length / 2.f}, {length / texSize, 0.f}},
        {{-width / 2.f,-height / 2.f,-length / 2.f}, {0.f,              height / texSize}},
        {{-width / 2.f,-height / 2.f, length / 2.f}, {length / texSize, height / texSize}},

        // Top face
        {{-width / 2.f, height / 2.f,-length / 2.f}, {0.f,             0.f}},
        {{ width / 2.f, height / 2.f,-length / 2.f}, {width / texSize, 0.f}},
        {{-width / 2.f, height / 2.f, length / 2.f}, {0.f,             length / texSize}},
        {{ width / 2.f, height / 2.f, length / 2.f}, {width / texSize, length / texSize}},

        // Bottom face
        {{-width / 2.f,-height / 2.f, length / 2.f}, {0.f,             0.f}},
        {{ width / 2.f,-height / 2.f, length / 2.f}, {width / texSize, 0.f}},
        {{-width / 2.f,-height / 2.f,-length / 2.f}, {0.f,             length / texSize}},
        {{ width / 2.f,-height / 2.f,-length / 2.f}, {width / texSize, length / texSize}},
    };

    static const GLuint indices[] = {
        0, 2, 1,
        1, 2, 3,

        4, 6, 5,
        5, 6, 7,

        8, 10, 9,
        9, 10, 11,

        12, 14, 13,
        13, 14, 15,

        16, 18, 17,
        17, 18, 19,

        20, 22, 21,
        21, 22, 23
    };

    calculate_buffers(vertices, 24, indices, 36);
}

