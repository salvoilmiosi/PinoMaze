#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "../engine/texture.h"

#include <memory>

#include <glm/glm.hpp>

struct color : public glm::vec3 {
	color() : glm::vec3() {}
	color(const glm::vec3 &v) : glm::vec3(v) {}
	color(const float r, const float g, const float b) : glm::vec3(r, g, b) {}

	color(const int hex) {
		r = ((hex & 0x00ff0000) >> (8 * 2)) / 255.f;
		g = ((hex & 0x0000ff00) >> (8 * 1)) / 255.f;
		b = ((hex & 0x000000ff) >> (8 * 0)) / 255.f;
	}
};

struct light {
    color ambient = 0x808080;
	color diffuse = 0xffffff;
	color specular = 0xffffff;

	glm::vec3 direction{ 0.f, 0.f, 1.f };
};

struct material {
	color ambient = 0xffffff;
	color diffuse = 0xffffff;
	color specular = 0x4c4c4c;
	color emissive = 0x000000;

	float shininess = 25.f;

	std::shared_ptr<texture> tex = nullptr;
	std::shared_ptr<texture> normals = nullptr;
	std::shared_ptr<texture> specmap = nullptr;
};

bool loadMaterials(const std::string &source);
void cleanupMaterials();

const std::shared_ptr<texture> getTexture(const std::string &name);
const std::shared_ptr<material> getMaterial(const std::string &name);

#endif  // __MATERIAL_H__