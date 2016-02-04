#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#include "texture.h"

class cubemap : public texture {
private:
	SDL_Surface *surfaces[6];

public:
    virtual ~cubemap();

public:
    bool loadSurfaces(SDL_Surface **surfaces);
    SDL_Surface *getSurface(unsigned int i);

    void bindTexture(int sampler);

public:
    bool loadSurface(SDL_Surface *surface) = delete;
    SDL_Surface *getSurface() = delete;

    virtual void clean();

	void setFilter(GLenum f);

	void setWrapParam(GLenum param);
};

#endif // __CUBEMAP_H__
