#include "material.h"
#include "res_loader.h"

material material::MAT_FLOOR;
material material::MAT_FLOOR_REFRACTED;
material material::MAT_BRICKS;
material material::MAT_COBBLE;
material material::MAT_TILES;
material material::MAT_PLASTER;
material material::MAT_MARBLE;
material material::MAT_MARBLE_REFRACTED;
material material::MAT_START;
material material::MAT_END;
material material::MAT_ARROW;
material material::MAT_RUST;

void material::cleanTexture() {
	if (tex) {
		tex->clean();
	}
	if (normals) {
		normals->clean();
	}
	tex = nullptr;
	normals = nullptr;
}

void material::loadMaterials() {
	static texture TEX_MARBLE_TEXTURE(loadImageFromResources(IDT_MARBLE_TEXTURE));
	static texture TEX_MARBLE_NORMALS(loadImageFromResources(IDT_MARBLE_NORMALS));
	static texture TEX_FLOOR_TEXTURE(loadImageFromResources(IDT_FLOOR_TEXTURE));
	static texture TEX_FLOOR_NORMALS(loadImageFromResources(IDT_FLOOR_NORMALS));
	static texture TEX_BRICKS_TEXTURE(loadImageFromResources(IDT_BRICKS_TEXTURE));
	static texture TEX_BRICKS_NORMALS(loadImageFromResources(IDT_BRICKS_NORMALS));
	static texture TEX_COBBLE_TEXTURE(loadImageFromResources(IDT_COBBLE_TEXTURE));
	static texture TEX_COBBLE_NORMALS(loadImageFromResources(IDT_COBBLE_NORMALS));
	static texture TEX_TILES_TEXTURE(loadImageFromResources(IDT_TILES_TEXTURE));
	static texture TEX_TILES_NORMALS(loadImageFromResources(IDT_TILES_NORMALS));
	static texture TEX_PLASTER_TEXTURE(loadImageFromResources(IDT_PLASTER_TEXTURE));
	static texture TEX_PLASTER_NORMALS(loadImageFromResources(IDT_PLASTER_NORMALS));
	static texture TEX_ARROW_TEXTURE(loadImageFromResources(IDT_ARROW_TEXTURE));
	static texture TEX_RUST_TEXTURE(loadImageFromResources(IDT_RUST_TEXTURE));
	static texture TEX_RUST_NORMALS(loadImageFromResources(IDT_RUST_NORMALS));

	MAT_FLOOR.specular = glm::vec3(0.15f, 0.15f, 0.15f);
	MAT_FLOOR.tex = &TEX_FLOOR_TEXTURE;
	MAT_FLOOR.normals = &TEX_FLOOR_NORMALS;

	MAT_FLOOR_REFRACTED.diffuse = glm::vec3(0.6f, 0.6f, 0.7f);
	MAT_FLOOR_REFRACTED.specular = glm::vec3(0.f, 0.f, 0.f);
	MAT_FLOOR_REFRACTED.tex = &TEX_FLOOR_TEXTURE;

	MAT_BRICKS.tex = &TEX_BRICKS_TEXTURE;
	MAT_BRICKS.normals = &TEX_BRICKS_NORMALS;

	MAT_COBBLE.tex = &TEX_COBBLE_TEXTURE;
	MAT_COBBLE.normals = &TEX_COBBLE_NORMALS;

	MAT_TILES.diffuse = glm::vec3(1.f, 1.f, 0.75f);
	MAT_TILES.specular = glm::vec3(0.4f, 0.4f, 0.4f);
	MAT_TILES.tex = &TEX_TILES_TEXTURE;
	MAT_TILES.normals = &TEX_TILES_NORMALS;

	MAT_PLASTER.ambient = glm::vec3(0.8f, 0.8f, 0.8f);
	MAT_PLASTER.specular = glm::vec3(0.2f, 0.2f, 0.2f);
	MAT_PLASTER.tex = &TEX_PLASTER_TEXTURE;
	MAT_PLASTER.normals = &TEX_PLASTER_NORMALS;

	MAT_MARBLE.ambient = glm::vec3(0.7f, 0.7f, 0.7f);
	MAT_MARBLE.specular = glm::vec3(0.6f, 0.6f, 0.6f);
	MAT_MARBLE.tex = &TEX_MARBLE_TEXTURE;
	MAT_MARBLE.normals = &TEX_MARBLE_NORMALS;

	MAT_MARBLE_REFRACTED.diffuse = glm::vec3(0.6f, 0.6f, 0.7f);
	MAT_MARBLE_REFRACTED.specular = glm::vec3(0.f, 0.f, 0.f);
	MAT_MARBLE_REFRACTED.tex = &TEX_MARBLE_TEXTURE;

	MAT_START.diffuse = glm::vec3(1.f, 0.f, 0.f);
	MAT_START.tex = &TEX_PLASTER_TEXTURE;
	MAT_START.normals = &TEX_PLASTER_NORMALS;

	MAT_END.diffuse = glm::vec3(1.f, 0.f, 1.f);
	MAT_END.tex = &TEX_PLASTER_TEXTURE;
	MAT_END.normals = &TEX_PLASTER_NORMALS;

	MAT_ARROW.tex = &TEX_ARROW_TEXTURE;
	MAT_ARROW.normals = &TEX_PLASTER_NORMALS;

	MAT_RUST.tex = &TEX_RUST_TEXTURE;
	MAT_RUST.normals = &TEX_RUST_NORMALS;
}

void material::cleanMaterials() {
	MAT_FLOOR.cleanTexture();
	MAT_BRICKS.cleanTexture();
	MAT_COBBLE.cleanTexture();
	MAT_TILES.cleanTexture();
	MAT_PLASTER.cleanTexture();
	MAT_MARBLE.cleanTexture();
	MAT_START.cleanTexture();
	MAT_END.cleanTexture();
	MAT_ARROW.cleanTexture();
}