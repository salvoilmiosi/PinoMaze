#ifndef __TOOL_STARTEND_H__
#define __TOOL_STARTEND_H__

#include "tool_base.h"

class toolStartend: public mazeEditorTool {
public:
    toolStartend(Uint8 sn) : mazeEditorTool(sn) {}

public:
    void handleEvent(SDL_Event &e) override;
};
#endif // __TOOL_STARTEND_H__
