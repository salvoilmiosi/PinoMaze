#ifndef __MODEL_BRIDGE_H__
#define __MODEL_BRIDGE_H__

#include <vector>

#include "../engine/base_model.h"
#include "../options.h"

class bridge {
public:
	bridge(class game *m_game);

public:
    void load_models(int gridx, int gridy, int gridsize);

    void render_flat();

    void render(class world_shader &m_shader);

private:
	class game *m_game;
	
	size_t under_arc_offset = 0;
	
	base_model m_arc;

	base_model m_wall[numWallMaterials];
};

#endif // __MODEL_BRIDGE_H__
