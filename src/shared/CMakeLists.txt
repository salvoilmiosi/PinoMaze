target_include_directories(PinoMaze_shared PUBLIC include)

target_sources(PinoMaze_shared PRIVATE
    a_star.cpp
    bit_array.cpp
    gui.cpp
    maze_items.cpp
    maze_open.cpp
    maze_save.cpp
    maze.cpp
    node.cpp
)