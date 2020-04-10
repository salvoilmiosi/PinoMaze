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

    ~texture();

public:
    int width() const {
        return w;
    }
    int height() const {
        return h;
    }

    void loadSurface(SDL_Surface *surface);

    void createEmpty(int width, int height, bool isDepth = false);

    SDL_Surface *getSurface();

    void setFilter(GLenum filter);
    void setWrapParam(GLenum wrapParam);

	void bindTexture() const;

    friend class framebuffer;
};

class sampler {
public:
    int gl_samplerid;

public:
    sampler(int gl_samplerid) : gl_samplerid(gl_samplerid) {}

    void bindTexture(const texture &t);
};

#endif // __TEXTURE_H__
