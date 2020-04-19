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

    SDL_Surface *m_surface = nullptr;

	int w = 0;
	int h = 0;

public:
    texture(SDL_Surface *surface = nullptr);
    texture(int width, int height, bool isDepth = false);

    texture(texture &&old);

    ~texture();

public:
    int width() const {
        return w;
    }

    int height() const {
        return h;
    }

    SDL_Surface *surface() const {
        return m_surface;
    }

    void setFilter(GLenum filter);
    void setWrapParam(GLenum wrapParam);

	void bind() const;

    friend class framebuffer;
};

class sampler {
public:
    sampler(int tex_unit) : tex_unit(tex_unit) {}
    
    void bind(const texture *t);

private:
    int tex_unit;

    friend class shader_program;
};

#endif // __TEXTURE_H__
