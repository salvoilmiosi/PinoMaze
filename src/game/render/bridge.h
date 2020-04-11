#ifndef __MODEL_BRIDGE_H__
#define __MODEL_BRIDGE_H__

#include <vector>

#include "../engine/base_model.h"

class bridge {
public:
	bridge();

public:
    void init(class maze *m_maze);

    void render_flat();

    void render(class world_shader &m_shader);

private:
	void addArcVerts(std::vector<base_vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float h, float texSize, bool ext);
	void addArcWallVerts(std::vector<base_vertex> &vertices, std::vector<GLuint> &indices, float z, float w, float texSize);
	void addTopWallVerts(std::vector<base_vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float texSize);
	void addIndices(std::vector<base_vertex> &vertices, std::vector<GLuint> &indices, GLuint startIndex, bool ext);

protected:
	size_t underArc_offset = 0;
	
	base_model m_arc;
	base_model m_wall;
};

#endif // __MODEL_BRIDGE_H__
