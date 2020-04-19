#include "world_shader.h"

#include "../resources.h"
#include "../game.h"

world_shader::world_shader(game *m_game) :
    vf_shader("world", SHADER_RESOURCE(s_world_v), SHADER_RESOURCE(s_world_f)),
    shadowMap(2048, 2048, true)
{
    add_uniform("projectionMatrix", &m_game->m_proj);
    add_uniform("viewMatrix", &m_game->m_view);
    add_uniform("lightMatrix", &m_light_biased);
    add_uniform("diffuseTexture", &diffuseSampler);
    add_uniform("normalTexture", &normalSampler);
    add_uniform("specularTexture", &specularSampler);
    add_uniform("tpTileTexture", &tpTileSampler);
    add_uniform("shadowMap", &shadowSampler);

    add_uniform("renderFlags", &renderFlags);
    
    add_uniform("sun.ambient", (glm::vec3 *) &m_sun.ambient);
    add_uniform("sun.diffuse", (glm::vec3 *) &m_sun.diffuse);
    add_uniform("sun.specular", (glm::vec3 *) &m_sun.specular);
    add_uniform("sun.direction", &m_sun.direction);

    add_uniform("mat.ambient", (glm::vec3 *) &m_material.ambient);
    add_uniform("mat.diffuse", (glm::vec3 *) &m_material.diffuse);
    add_uniform("mat.specular", (glm::vec3 *) &m_material.specular);
    add_uniform("mat.emissive", (glm::vec3 *) &m_material.emissive);
    add_uniform("mat.shininess", &m_material.shininess);

    add_uniform("shadowBias", &shadowBias);
    add_uniform("shadowTexelSize", &shadowTexelSize);
    add_uniform("refractionHeight", &refractionHeight);

    shadowMap.setFilter(GL_NEAREST);
    shadowMap.setWrapParam(GL_CLAMP_TO_EDGE);
    shadowBuffer.attachDepthMap(shadowMap);
    
    shadowBias = 0.003f;
    shadowTexelSize = glm::vec2(1.f / shadowMap.width());

    addFlags(ENABLE_SHADOWS);
}

void world_shader::apply_material(const char *mat_name) {
    auto mat = getMaterial(mat_name);
    if (mat) m_material = *mat;
    else m_material = {};
    if (m_material.tex) {
        diffuseSampler.bind(m_material.tex.get());
        addFlags(ENABLE_TEXTURE);
    } else {
        removeFlags(ENABLE_TEXTURE);
    }
    if (m_material.normals) {
        normalSampler.bind(m_material.normals.get());
        addFlags(ENABLE_NORMALS);
    } else {
        removeFlags(ENABLE_NORMALS);
    }
    if (m_material.specmap) {
        specularSampler.bind(m_material.specmap.get());
        addFlags(ENABLE_SPECULAR);
    } else {
        removeFlags(ENABLE_SPECULAR);
    }
    update_uniforms();
}