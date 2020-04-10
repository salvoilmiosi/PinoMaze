#ifndef __MODEL_BRIDGE_H__
#define __MODEL_BRIDGE_H__

#include "../engine/model.h"
#include "maze.h"

class bridge {
public:
	bridge();

public:
    void init(maze *m_maze);

    void drawFlat();

    void draw(class shader &m_shader, class material &m_material);

private:
	void drawArc(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float h, float texSize, bool ext);
	void drawArcWall(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z, float w, float texSize);
	void drawTopWall(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float texSize);
	void addIndices(std::vector<vertex> &vertices, std::vector<GLuint> &indices, GLuint startIndex, bool ext);

protected:
    size_t overArc_offset = 0;
	size_t underArc_offset = 0;
	
	model m_arc;
	model m_wall;
};

#endif // __MODEL_BRIDGE_H__
