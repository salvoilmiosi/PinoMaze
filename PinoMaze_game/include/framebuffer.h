#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

class framebuffer {
private:
    GLuint fboID = 0;

    int width = 0;
    int height = 0;

public:
    virtual ~framebuffer();

public:
    bool init();

    void bindFramebuffer();
    void unbindFramebuffer();

public:
    void attachTexture(const texture &t);
    void attachDepthMap(const texture &t);
};

#endif // __FRAMEBUFFER_H__
