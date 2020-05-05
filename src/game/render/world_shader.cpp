#include "world_shader.h"

#include "../resources.h"
#include "../game.h"

world_shader::world_shader(game *m_game) :
    vf_shader("world", SHADER_RESOURCE(s_world_v), SHADER_RESOURCE(s_world_f)),
    shadowMap(2048, 2048, true)
{
    add_uniforms(
        "projectionMatrix", &m_game->m_proj,
        "viewMatrix", &m_game->m_view,
        "lightMatrix", &m_light_biased,
        "diffuseTexture", &diffuseSampler,
        "normalTexture", &normalSampler,
        "specularTexture", &specularSampler,
        "tpTileTexture", &tpTileSampler,
        "shadowMap", &shadowSampler,

        "renderFlags", &renderFlags,

        "shadowBias", &shadowBias,
        "shadowTexelSize", &shadowTexelSize,
        "refractionHeight", &refractionHeight,
        
        "sun.ambient", (glm::vec3 *) &m_sun.ambient,
        "sun.diffuse", (glm::vec3 *) &m_sun.diffuse,
        "sun.specular", (glm::vec3 *) &m_sun.specular,
        "sun.direction", &m_sun.direction,

        "mat.ambient", (glm::vec3 *) &m_material.ambient,
        "mat.diffuse", (glm::vec3 *) &m_material.diffuse,
        "mat.specular", (glm::vec3 *) &m_material.specular,
        "mat.emissive", (glm::vec3 *) &m_material.emissive,
        "mat.shininess", &m_material.shininess
    );

    shadowMap.setFilter(GL_NEAREST);
    shadowMap.setWrapParam(GL_CLAMP_TO_EDGE);
    shadowBuffer.attachDepthMap(shadowMap);
    
    shadowBias = 0.003f;
    shadowTexelSize = glm::vec2(1.f / shadowMap.width());

    addFlags(ENABLE_SHADOWS);
}

void world_shader::apply_material(const char *mat_name) {
    auto mat = materials::getMaterial(mat_name);
    if (mat) m_material = *mat;
    else m_material = {};
    if (m_material.tex) {
        m_material.tex->bindTo(diffuseSampler);
        addFlags(ENABLE_TEXTURE);
    } else {
        removeFlags(ENABLE_TEXTURE);
    }
    if (m_material.normals) {
        m_material.normals->bindTo(normalSampler);
        addFlags(ENABLE_NORMALS);
    } else {
        removeFlags(ENABLE_NORMALS);
    }
    if (m_material.specmap) {
        m_material.specmap->bindTo(specularSampler);
        addFlags(ENABLE_SPECULAR);
    } else {
        removeFlags(ENABLE_SPECULAR);
    }
    update_uniforms();
}