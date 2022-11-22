
project(resize_image
    VERSION 0.0.1
    LANGUAGES C
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/gui_sdl_ui.c
    ${cTester_SOURCE_DIR}/gui_sdl.c
    ${cTester_SOURCE_DIR}/gui_sdl_widget.c
)
target_include_directories(${PROJECT_NAME} PRIVATE ${cTester_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${lua_SOURCE_DIR}/src)
target_link_directories(${PROJECT_NAME} PRIVATE ${lua_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE lua)
add_dependencies(${PROJECT_NAME} lua_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2main)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image)
