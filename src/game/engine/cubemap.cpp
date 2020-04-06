#include "cubemap.h"

cubemap::~cubemap() {
	for (int i = 0; i<6; ++i) {
		SDL_FreeSurface(surfaces[i]);
	}
}

bool cubemap::loadSurfaces(SDL_Surface **surfs) {
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (int i=0; i<6; ++i) {
		surfaces[i] = surfs[i];
        unsigned char bpp = surfaces[i]->format->BytesPerPixel;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, bpp, surfaces[i]->w, surfaces[i]->h, 0,
                     bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, surfaces[i]->pixels);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapParam);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapParam);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapParam);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}

SDL_Surface *cubemap::getSurface(unsigned int i) {
    if (i < 6)
        return surfaces[i];
    return nullptr;
}

void cubemap::bindTexture(int sampler) {
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
}

void cubemap::setFilter(GLenum f) {
	filter = f;
	if (texID != 0) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

void cubemap::setWrapParam(GLenum param) {
	wrapParam = param;
	if (texID != 0) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapParam);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapParam);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapParam);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}