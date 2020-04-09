#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

#include "context.h"

class framebuffer {
private:
    GLuint fboID = 0;

    int width = 0;
    int height = 0;

public:
    framebuffer();
    ~framebuffer();

public:
    void bindFramebuffer();
    static void unbindFramebuffer(context *con);

public:
    void attachTexture(const texture &t);
    void attachDepthMap(const texture &t);
};

#endif // __FRAMEBUFFER_H__
