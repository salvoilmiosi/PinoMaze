#include "world_shader.h"

#include "../resources.h"
#include "../game.h"

world_shader::world_shader(game *m_game) :
    shader("world", SHADER_RESOURCE(s_world_v), SHADER_RESOURCE(s_world_f)),
    shadowMap(4096, 4096, true)
{
    add_uniform("projectionMatrix", &m_game->m_proj);
    add_uniform("viewMatrix", &m_game->m_view);
    add_uniform("lightMatrix", &m_light_biased);
    add_uniform("diffuseTexture", &diffuseSampler.gl_samplerid);
    add_uniform("normalTexture", &normalSampler.gl_samplerid);
    add_uniform("tpTileTexture", &tpTileSampler.gl_samplerid);
    add_uniform("shadowMap", &shadowSampler.gl_samplerid);

    add_uniform("enableTexture", &enableTexture);
    add_uniform("enableNormalMap", &enableNormalMap);
    add_uniform("enableTpTiles", &enableTpTiles);
    add_uniform("enableShadow", &enableShadow);
    add_uniform("enableSpecular", &enableSpecular);
    
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
    add_uniform("tpTileSize", &tpTileSize);

    shadowMap.setFilter(GL_NEAREST);
    shadowMap.setWrapParam(GL_CLAMP_TO_EDGE);
    shadowBuffer.attachDepthMap(shadowMap);
    
    shadowBias = 0.003f;
    shadowTexelSize = glm::vec2(1.f / shadowMap.width());
}

void world_shader::apply_material(const char *mat_name) {
    m_material = material::get(mat_name);
    diffuseSampler.bind(material::getTexture(m_material.tex));
    normalSampler.bind(material::getTexture(m_material.normals));
    update_uniforms();
}