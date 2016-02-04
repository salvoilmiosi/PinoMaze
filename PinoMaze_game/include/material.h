#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "texture.h"

#include <glm/glm.hpp>

class material {
public:
	static material MAT_FLOOR;
	static material MAT_FLOOR_REFRACTED;
	static material MAT_BRICKS;
	static material MAT_COBBLE;
	static material MAT_TILES;
	static material MAT_PLASTER;
	static material MAT_MARBLE;
	static material MAT_MARBLE_REFRACTED;
	static material MAT_START;
	static material MAT_END;
	static material MAT_ARROW;
	static material MAT_RUST;

	static void loadMaterials();
	static void cleanMaterials();

public:
	glm::vec3 ambient{ 1.f, 1.f, 1.f };
	glm::vec3 diffuse{ 1.f, 1.f, 1.f };
	glm::vec3 specular{ 0.3f, 0.3f, 0.3f };
	glm::vec3 emissive{ 0.f, 0.f, 0.f };
	float shininess = 25.f;

    texture *tex = nullptr;
    texture *normals = nullptr;

private:
	void cleanTexture();
};

#endif // __MATERIAL_H__
