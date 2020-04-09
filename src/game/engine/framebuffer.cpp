#include "framebuffer.h"

#include "context.h"

framebuffer::framebuffer() {
    glGenFramebuffers(1, &fboID);

	checkGlError("Failed to init framebuffer");
}

framebuffer::~framebuffer() {
	glDeleteFramebuffers(1, &fboID);
}

void framebuffer::bindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);
}

void framebuffer::unbindFramebuffer(context *con) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, con->window_width, con->window_width);
}

void framebuffer::attachTexture(const texture &t) {
    bindFramebuffer();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.texID, 0);
    width = t.width();
    height = t.height();
}

void framebuffer::attachDepthMap(const texture &t) {
    bindFramebuffer();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, t.texID, 0);
    width = t.width();
    height = t.height();
}
