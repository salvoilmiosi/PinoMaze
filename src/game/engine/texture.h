#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

class texture {
protected:
    GLuint texID = 0;
    GLenum filter = GL_LINEAR;
    GLenum wrapParam = GL_REPEAT;

    SDL_Surface *surface = nullptr;

	int w = 0;
	int h = 0;

public:
	texture() {}

	texture(SDL_Surface *surf) {
		loadSurface(surf);
	}

    virtual ~texture();

public:
    int width() const {
        return w;
    }
    int height() const {
        return h;
    }

    bool loadSurface(SDL_Surface *surface);
	bool loadSurfaceFromResource(const char *RES_ID);

    bool createEmpty(int width, int height, bool isDepth = false);

    SDL_Surface *getSurface();

    void setFilter(GLenum filter);
    void setWrapParam(GLenum wrapParam);

    void bindTexture(unsigned int sampler);
	void bindTexture();

    friend class framebuffer;
};

#endif // __TEXTURE_H__
