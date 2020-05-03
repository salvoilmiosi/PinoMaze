#include "framebuffer.h"

framebuffer::framebuffer() {
    glGenFramebuffers(1, &fboID);
}

framebuffer::~framebuffer() {
	glDeleteFramebuffers(1, &fboID);
}

void framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);
}

void framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::attachTexture(const texture &t) {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.texID, 0);
    width = t.width();
    height = t.height();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::attachDepthMap(const texture &t) {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, t.texID, 0);
    width = t.width();
    height = t.height();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool framebuffer::complete() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}