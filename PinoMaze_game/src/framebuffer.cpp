#include "framebuffer.h"
#include "globals.h"

framebuffer::~framebuffer() {
    clean();
}

bool framebuffer::init() {
    glGenFramebuffers(1, &fboID);

    return glGetError() == GL_NO_ERROR;
}

void framebuffer::clean() {
    glDeleteFramebuffers(1, &fboID);
}

void framebuffer::bindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);
}

void framebuffer::unbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
}

void framebuffer::attachTexture(const texture &t) {
    bindFramebuffer();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.texID, 0);
    width = t.width();
    height = t.height();
    unbindFramebuffer();
}

void framebuffer::attachDepthMap(const texture &t) {
    bindFramebuffer();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, t.texID, 0);
    width = t.width();
    height = t.height();
    unbindFramebuffer();
}
