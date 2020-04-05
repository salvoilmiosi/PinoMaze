#ifndef __MODEL_BRIDGE_H__
#define __MODEL_BRIDGE_H__

#include "../engine/model.h"
#include "maze.h"

class bridgeRenderer: public model {
protected:
    GLuint arcVAO = 0;
    GLuint wallVAO = 0;

    GLuint arcMatrixVBO = 0;
    GLuint wallMatrixVBO = 0;

    std::vector<glm::mat4> wallMatrices;
protected:
    size_t overArc_offset = 0;
	size_t underArc_offset = 0;
	size_t wall_offset = 0;
	size_t index_count = 0;

    std::vector<vertex> vertices;
    std::vector<GLuint> indices;

private:
    maze *m = nullptr;

public:
	bridgeRenderer(maze *m);
	virtual ~bridgeRenderer();

public:
    bool init();

    void render();

    void renderShader(class game *g, class worldShader &shader);

private:
	void drawArc(float z1, float z2, float w, float h, float texSize, bool ext);
	void drawArcWall(float z, float w, float texSize);
	void drawTopWall(float z1, float z2, float w, float texSize);
	void addIndices(GLuint startIndex, bool ext);
};

#endif // __MODEL_BRIDGE_H__
