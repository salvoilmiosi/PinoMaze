#ifndef __MODEL_BRIDGE_H__
#define __MODEL_BRIDGE_H__

#include <vector>

#include "../engine/base_model.h"
#include "../options.h"

class bridge {
public:
	bridge();

public:
    void init(class maze *m_maze);

    void render_flat();

    void render(class world_shader &m_shader);

protected:
	size_t underArc_offset = 0;
	
	base_model m_arc;

	base_model m_wall[numWallMaterials];
};

#endif // __MODEL_BRIDGE_H__
