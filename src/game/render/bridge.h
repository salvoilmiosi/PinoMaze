#ifndef __MODEL_BRIDGE_H__
#define __MODEL_BRIDGE_H__

#include <vector>

#include "../engine/model.h"

class bridge {
public:
	bridge();

public:
    void init(class maze *m_maze);

    void drawFlat();

    void draw(class world_shader &m_shader);

private:
	void addArcVerts(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float h, float texSize, bool ext);
	void addArcWallVerts(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z, float w, float texSize);
	void addTopWallVerts(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float texSize);
	void addIndices(std::vector<vertex> &vertices, std::vector<GLuint> &indices, GLuint startIndex, bool ext);

protected:
	size_t underArc_offset = 0;
	
	model m_arc;
	model m_wall;
};

#endif // __MODEL_BRIDGE_H__
