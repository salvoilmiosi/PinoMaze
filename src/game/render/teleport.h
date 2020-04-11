#ifndef __TELEPORT_H__
#define __TELEPORT_H__

#include "../engine/box.h"
#include "../engine/texture.h"

class teleport : public box {
public:
    teleport();

    void init(class maze *m_maze);

public:
    texture tex;
};

#endif