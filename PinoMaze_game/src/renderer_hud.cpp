#include "renderer_hud.h"

#include "res_loader.h"
#include "globals.h"

hudRenderer::~hudRenderer() {
    clean();
}

glm::vec2 hudRenderer::toWorldCoords(int x, int y) {
    return glm::vec2(-1.f, 1.f) + toWorldVector(x, y);
}

glm::vec2 hudRenderer::toWorldVector(int x, int y) {
    return glm::vec2(x * 2.f / windowWidth, -y * 2.f / windowHeight);
}

bool hudRenderer::init() {
    if (!shader.init()) return false;

	TEX_FONT_TEXTURE.loadSurface(loadImageFromResources(IDT_FONT_TEXTURE));
	TEX_FONT_TEXTURE.setFilter(GL_NEAREST);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE_DEFAULT * sizeof(vertex), nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertex), (void*)(2*sizeof(float)));

    glBindVertexArray(0);

    return glGetError() == GL_NO_ERROR;
}

void hudRenderer::setStatus(const char *str) {
	text = str;
	textChanged = true;
}

void hudRenderer::clean() {
    glBindVertexArray(vertexArray);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteVertexArrays(1, &vertexArray);

    glDeleteBuffers(1, &vertexBuffer);

	TEX_FONT_TEXTURE.clean();
}

int hudRenderer::buildTextBuffer() {
	struct rectf {
		float x, y;
		float w, h;
	};

	glm::vec2 topleft = toWorldCoords(10, 10);
	glm::vec2 dstsize = toWorldVector(8, 8);

	rectf srcrect = { 0.f, 0.f, 1.f / 16.f, 1.f / 16.f };
	rectf dstrect = { topleft.x, topleft.y, dstsize.x, -dstsize.y };

	unsigned int i = 0;

	for (string::iterator c = text.begin(); c != text.end(); ++c) {
		if (*c == '\0') {
			break;
		} else if (*c == '\n') {
			dstrect.x = topleft.x;
			dstrect.y -= dstrect.h;
		} else {
			srcrect.x = (*c % 16) * srcrect.w;
			srcrect.y = (*c / 16) * srcrect.h;

			vertex v1 = { dstrect.x, dstrect.y, srcrect.x, srcrect.y };
			vertex v2 = { dstrect.x, dstrect.y - dstrect.h, srcrect.x, srcrect.y + srcrect.h };
			vertex v3 = { dstrect.x + dstrect.w, dstrect.y, srcrect.x + srcrect.w, srcrect.y };
			vertex v4 = { dstrect.x + dstrect.w, dstrect.y - dstrect.h, srcrect.x + srcrect.w, srcrect.y + srcrect.h };

			dstrect.x += dstrect.w;
			if (i < vertices.size()) {
				vertices[i] = v1;
				vertices[i + 1] = v2;
				vertices[i + 2] = v3;
				vertices[i + 3] = v3;
				vertices[i + 4] = v2;
				vertices[i + 5] = v4;
			} else {
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);
				vertices.push_back(v3);
				vertices.push_back(v2);
				vertices.push_back(v4);
			}
			i += 6;
		}
	}

	static unsigned int maxSize = BUFFER_SIZE_DEFAULT;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	if (i > maxSize) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * i, vertices.data(), GL_DYNAMIC_DRAW);
		maxSize = i;
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex) * i, vertices.data());
	}

	return i;
}

void hudRenderer::render() {
	if (shader.bindProgram()) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		static int numVertices = 0;
		if (textChanged) {
			numVertices = buildTextBuffer();
			textChanged = false;
		}

		glBindVertexArray(vertexArray);
		TEX_FONT_TEXTURE.bindTexture(0);
		shader.setDiffuseColor(glm::vec3(0.f, 0.f, 0.f));
		shader.setOffset(toWorldVector(1, 1));
		glDrawArrays(GL_TRIANGLES, 0, numVertices);

		shader.setDiffuseColor(glm::vec3(1.f, 0.f, 0.f));
		shader.setOffset(glm::vec2(0.f, 0.f));
		glDrawArrays(GL_TRIANGLES, 0, numVertices);

		glDisable(GL_BLEND);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		shader.unbindProgram();
	}
}
