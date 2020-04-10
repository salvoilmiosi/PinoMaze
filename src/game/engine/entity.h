#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "context.h"

class entity {
public:
    entity(context *m_context) : m_context(m_context) {}

    virtual void tick() {}
    virtual void render() {}

protected:
    context *m_context;
};

#endif // __ENTITY_H__