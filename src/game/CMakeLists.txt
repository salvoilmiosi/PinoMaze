add_subdirectory(engine)
add_subdirectory(render)
add_subdirectory(shader)

target_sources(PinoMaze_game PRIVATE
    game.cpp
    main.cpp
    resources.cpp
)