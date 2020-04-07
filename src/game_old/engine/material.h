#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "../resources.h"

#include "texture.h"

#include <string>
#include <memory>
#include <map>

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

class material {
public:
	static const material MAT_DEFAULT;

	color ambient = 0xffffff;
	color diffuse = 0xffffff;
	color specular = 0x4c4c4c;
	color emissive = 0x000000;

	float shininess = 25.f;

	std::shared_ptr<texture> tex = nullptr;
	std::shared_ptr<texture> normals = nullptr;

	material() {}

	material(color ambient, color diffuse, color specular, color emissive,
		std::shared_ptr<texture> tex = nullptr, std::shared_ptr<texture> normals = nullptr)
		: ambient(ambient), diffuse(diffuse), specular(specular), emissive(emissive), tex(tex), normals(normals) {}

	material(std::shared_ptr<texture> tex, std::shared_ptr<texture> normals) : tex(tex), normals(normals) {}

public:
	static bool loadMaterials(const std::string &source);

	static const material &get(const char *name);

private:
	static std::map<std::string, material> mat;
};

#endif  // __MATERIAL_H__