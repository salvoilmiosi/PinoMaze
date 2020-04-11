#include "teleport.h"

#include "maze.h"
#include "world_shader.h"

#include "../options.h"
#include "../resources.h"

teleport::teleport() : box(startBoxSize, startBoxHeight, startBoxSize, startBoxSize) {
	tex.setFilter(GL_NEAREST);
	tex.loadSurface(loadImageFromResource("IDT_TELEPORT_TEXTURE"));
}

void teleport::init(maze *m_maze) {
    struct instance_data {
        glm::mat4 matrix;
        glm::vec2 uv;
    };

    std::vector<instance_data> instances;

	for (std::pair<const int, mazeItem> &it : m_maze->items) {
		if (it.second.type == ITEM_TELEPORT) {
            instance_data data;

			unsigned char c = it.second.teleport.tpChar;
			data.uv.x = (1.f / 16.f) * (c % 16);
			data.uv.y = (1.f / 16.f) * (c / 16);

			int x = it.second.item.x;
			int y = it.second.item.y;

			data.matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));

            instances.push_back(data);
		}
	}

    vao.update_instances(2, instances.data(), instances.size() * sizeof(instance_data), {{4, ATTR_MAT4}, {8, ATTR_VEC2}});
}