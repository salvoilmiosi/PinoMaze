function(compile_resources target_name)
    foreach(file IN LISTS ARGN)
        get_filename_component(varname "${file}" NAME)
        string(MAKE_C_IDENTIFIER "${varname}" varname)

        set(resource_c_file "${CMAKE_CURRENT_BINARY_DIR}/${file}.c")
        list(APPEND out_list "${resource_c_file}")

        add_custom_command(
            OUTPUT "${resource_c_file}"
            COMMAND ${CMAKE_COMMAND}
                -D "resource_file_name=${CMAKE_CURRENT_SOURCE_DIR}/${file}"
                -D "source_file_name=${resource_c_file}"
                -D "variable_name=__resource__${varname}"
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/EmbedResource.cmake"
            DEPENDS "${file}"
            VERBATIM
        )
    endforeach()
    add_library(${target_name} OBJECT ${out_list})
endfunction()

compile_resources(PinoMaze_materials materials.txt)
target_link_libraries(PinoMaze_game PRIVATE PinoMaze_materials)

function(pack_resources TARGET_NAME)
    foreach(filename IN LISTS ARGN)
        set(txt_file "${CMAKE_CURRENT_SOURCE_DIR}/${filename}")
        set(dat_file "${CMAKE_BINARY_DIR}/${filename}")
        string(REGEX REPLACE "[.]txt$" ".dat" dat_file "${dat_file}")
        add_custom_command(
            OUTPUT "${dat_file}"
            COMMAND resource_pack "${txt_file}" "${dat_file}"
            VERBATIM
            DEPENDS "${txt_file}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        list(APPEND dat_files "${dat_file}")
    endforeach()
    add_custom_target("${TARGET_NAME}" DEPENDS "${dat_files}")
endfunction()

pack_resources(editor_resources editor.txt)
add_dependencies(PinoMaze_editor editor_resources)
if(WIN32)
    target_sources(PinoMaze_editor PRIVATE editor.rc)
endif()

pack_resources(game_resources resource.txt music.txt)
add_dependencies(PinoMaze_game game_resources)
if(WIN32)
    target_sources(PinoMaze_game PRIVATE game.rc)
endif()