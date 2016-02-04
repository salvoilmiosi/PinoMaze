#ifndef __TOOL_STARTEND_H__
#define __TOOL_STARTEND_H__

#include "tool_base.h"

class toolStartend: public mazeEditorTool {
public:
    toolStartend(Uint8 sn) : mazeEditorTool(sn) {}
    virtual ~toolStartend() {}

public:
    void handleEvent(SDL_Event &e);
};
#endif // __TOOL_STARTEND_H__
