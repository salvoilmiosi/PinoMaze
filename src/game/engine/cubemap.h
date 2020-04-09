#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#include "texture.h"

class cubemap : public texture {
private:
	SDL_Surface *surfaces[6];

public:
    ~cubemap();

public:
    void loadSurfaces(SDL_Surface **surfaces);
    SDL_Surface *getSurface(unsigned int i);

    void bindTexture(int sampler);

public:
    void loadSurface(SDL_Surface *surface) = delete;
    SDL_Surface *getSurface() = delete;

	void setFilter(GLenum f);

	void setWrapParam(GLenum param);
};

#endif // __CUBEMAP_H__
