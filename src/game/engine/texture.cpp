#include "texture.h"

#include "context.h"

texture::~texture() {
	glDeleteTextures(1, &texID);
	if (surface != nullptr)
		SDL_FreeSurface(surface);
	surface = nullptr;
}

void texture::loadSurface(SDL_Surface *_surface) {
    surface = _surface;

    if (surface == nullptr) return;
    unsigned char bpp = surface->format->BytesPerPixel;
    if (bpp != 3 && bpp != 4) return;

    w = surface->w;
    h = surface->h;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, bpp, surface->w, surface->h, 0,
                 bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);

	checkGlError("Failed to load texture");
}

void texture::createEmpty(int width, int height, bool isDepth) {
    w = width;
    h = height;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, isDepth ? GL_DEPTH_COMPONENT16 : GL_RGBA8,
                 width, height, 0, isDepth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);

	glBindTexture(GL_TEXTURE_2D, 0);

	checkGlError("Failed to create texture");
}

SDL_Surface *texture::getSurface() {
    return surface;
}

void texture::setFilter(GLenum f) {
    filter = f;
	if (texID != 0) {
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void texture::setWrapParam(GLenum p) {
    wrapParam = p;
	if (texID != 0) {
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void texture::bindTexture() const {
	glBindTexture(GL_TEXTURE_2D, texID);
}

void sampler::bindTexture(const texture &t) {
    glActiveTexture(GL_TEXTURE0 + gl_samplerid);
	t.bindTexture();
}