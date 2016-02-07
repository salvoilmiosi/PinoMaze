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

const glm::vec3 colorToVec3(const int color) {
	const int r = (color & 0x00ff0000) >> (8 * 2);
	const int g = (color & 0x0000ff00) >> (8 * 1);
	const int b = (color & 0x000000ff) >> (8 * 0);
	return glm::vec3(r / 255.f, g / 255.f, b / 255.f);
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

	MAT_FLOOR.specular = colorToVec3(0x262626);
	MAT_FLOOR.tex = &TEX_FLOOR_TEXTURE;
	MAT_FLOOR.normals = &TEX_FLOOR_NORMALS;

	MAT_FLOOR_REFRACTED.diffuse = colorToVec3(0x9999b2);
	MAT_FLOOR_REFRACTED.specular = colorToVec3(0x000000);
	MAT_FLOOR_REFRACTED.tex = &TEX_FLOOR_TEXTURE;

	MAT_BRICKS.tex = &TEX_BRICKS_TEXTURE;
	MAT_BRICKS.normals = &TEX_BRICKS_NORMALS;

	MAT_COBBLE.tex = &TEX_COBBLE_TEXTURE;
	MAT_COBBLE.normals = &TEX_COBBLE_NORMALS;

	MAT_TILES.diffuse = colorToVec3(0xffffbf);
	MAT_TILES.specular = colorToVec3(0x666666);
	MAT_TILES.tex = &TEX_TILES_TEXTURE;
	MAT_TILES.normals = &TEX_TILES_NORMALS;

	MAT_PLASTER.ambient = colorToVec3(0xffffff);
	MAT_PLASTER.diffuse = colorToVec3(0xaea4a9);
	MAT_PLASTER.specular = colorToVec3(0x333333);
	MAT_PLASTER.tex = &TEX_PLASTER_TEXTURE;
	MAT_PLASTER.normals = &TEX_PLASTER_NORMALS;

	MAT_MARBLE.ambient = colorToVec3(0xb2b2b2);
	MAT_MARBLE.specular = colorToVec3(0x999999);
	MAT_MARBLE.tex = &TEX_MARBLE_TEXTURE;
	MAT_MARBLE.normals = &TEX_MARBLE_NORMALS;

	MAT_MARBLE_REFRACTED.diffuse = colorToVec3(0x9999b2);
	MAT_MARBLE_REFRACTED.specular = colorToVec3(0x000000);
	MAT_MARBLE_REFRACTED.tex = &TEX_MARBLE_TEXTURE;

	MAT_START.diffuse = colorToVec3(0xff0000);
	MAT_START.tex = &TEX_PLASTER_TEXTURE;
	MAT_START.normals = &TEX_PLASTER_NORMALS;

	MAT_END.diffuse = colorToVec3(0xff00ff);
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
