#include "texture.h"

#include "context.h"

#include <iostream>

texture::texture(SDL_Surface *m_surface) : m_surface(m_surface) {
    if (m_surface == nullptr) return;

    unsigned char bpp = m_surface->format->BytesPerPixel;
    if (bpp != 3 && bpp != 4) return;

    w = m_surface->w;
    h = m_surface->h;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, bpp, m_surface->w, m_surface->h, 0,
                 bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, m_surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);

	checkGlError("Failed to load texture");
}

texture::texture(int width, int height, bool isDepth) {
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

texture::texture(texture &&old) {
    texID = old.texID;
    old.texID = 0;
    filter = old.filter;
    wrapParam = old.wrapParam;
    m_surface = old.m_surface;
    old.m_surface = nullptr;
    w = old.w;
    h = old.h;
}

texture::~texture() {
	if (texID) glDeleteTextures(1, &texID);
	if (m_surface) SDL_FreeSurface(m_surface);
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

void texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, texID);
}

void sampler::bind(const texture *t) {
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    if (t) {
        t->bind();
    } else {
		glBindTexture(GL_TEXTURE_2D, 0);
    }
}