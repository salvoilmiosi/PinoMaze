compile_resources(PinoMaze_shaders
    s_billboard_f.glsl
    s_billboard_g.glsl
    s_billboard_v.glsl
    s_hud_f.glsl
    s_hud_v.glsl
    s_particle_g.glsl
    s_particle_v.glsl
    s_shadow_f.glsl
    s_shadow_v.glsl
    s_skybox_f.glsl
    s_skybox_v.glsl
    s_water_f.glsl
    s_water_v.glsl
    s_world_f.glsl
    s_world_v.glsl
)

target_link_libraries(PinoMaze_game PRIVATE PinoMaze_shaders)