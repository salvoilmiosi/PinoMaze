#include "hud.h"

#include <string>
#include <vector>

#include "../resources.h"
#include "../options.h"

#include "material.h"

hud::hud(context *m_context) : m_context(m_context),
	m_shader("hud", SHADER_RESOURCE(s_hud_v), SHADER_RESOURCE(s_hud_f))
{
	m_shader.add_uniform("offset", &m_offset);
	m_shader.add_uniform("diffuseTexture", &m_sampler);
	m_shader.add_uniform("diffuseColor", &m_color);

	checkGlError("Failed to init HUD");
}

glm::vec2 hud::toWorldCoords(int x, int y) {
    return glm::vec2(-1.f, 1.f) + toWorldVector(x, y);
}

glm::vec2 hud::toWorldVector(int x, int y) {
    return glm::vec2(x * 2.f / m_context->window_width, -y * 2.f / m_context->window_height);
}

float calculate_fps(float deltaNano) {
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	frameTimes[currentFrame % NUM_SAMPLES] = deltaNano;

	++currentFrame;
	size_t count = (currentFrame < NUM_SAMPLES) ? currentFrame : NUM_SAMPLES;
	float frameTimeAverage = 0;
	for (size_t i=0; i<count; ++i) {
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;
	return 1000000000.f / frameTimeAverage;
}

void hud::render(float deltaNano) {
	std::string text = std::to_string((int) calculate_fps(deltaNano));
	
	struct hud_vertex {
		float x, y;
		float u, v;
	};

	struct rectf {
		float x, y;
		float w, h;
	};

	std::vector<hud_vertex> vertices;

	glm::vec2 topleft = toWorldCoords(10, 10);
	glm::vec2 dstsize = toWorldVector(8, 8);

	rectf srcrect = { 0.f, 0.f, 1.f / 16.f, 1.f / 16.f };
	rectf dstrect = { topleft.x, topleft.y, dstsize.x, -dstsize.y };

	unsigned int i = 0;

	for (std::string::iterator c = text.begin(); c != text.end(); ++c) {
		if (*c == '\0') {
			break;
		} else if (*c == '\n') {
			dstrect.x = topleft.x;
			dstrect.y -= dstrect.h;
		} else {
			srcrect.x = (*c % 16) * srcrect.w;
			srcrect.y = (*c / 16) * srcrect.h;

			hud_vertex v1 = { dstrect.x, dstrect.y, srcrect.x, srcrect.y };
			hud_vertex v2 = { dstrect.x, dstrect.y - dstrect.h, srcrect.x, srcrect.y + srcrect.h };
			hud_vertex v3 = { dstrect.x + dstrect.w, dstrect.y, srcrect.x + srcrect.w, srcrect.y };
			hud_vertex v4 = { dstrect.x + dstrect.w, dstrect.y - dstrect.h, srcrect.x + srcrect.w, srcrect.y + srcrect.h };

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

	vao.update_vertices(0, vertices.data(), vertices.size() * sizeof(hud_vertex), {{0, ATTR_VEC2}, {1, ATTR_VEC2}}, true);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_sampler.bind(getTexture("TEX_FONT_TEXTURE").get());

	m_color = glm::vec3(0.f, 0.f, 0.f);
	m_offset = toWorldVector(1, 1);
	m_shader.use();
	vao.draw();

	m_color = glm::vec3(1.f, 0.f, 0.f);
	m_offset = glm::vec2(0.f, 0.f);
	m_shader.update_uniforms();
	vao.draw();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}