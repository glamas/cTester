cmake_minimum_required(VERSION 3.14..3.22)


project(cTester
    VERSION 0.0.1
    LANGUAGES C
)
set(CMAKE_BUILD_TYPE Release)

include(FetchContent)

FetchContent_Declare(
  lua
  URL https://www.lua.org/ftp/lua-5.4.4.tar.gz
  URL_HASH SHA256=164c7849653b80ae67bec4b7473b884bf5cc8d2dca05653475ec2ed27b9ebf61
)
FetchContent_MakeAvailable(lua)

add_custom_command(
    OUTPUT "${lua_SOURCE_DIR}/src/liblua.a"
    COMMAND cd ${lua_SOURCE_DIR} && mingw32-make mingw PLAT=mingw CC=clang
    WORKING_DIRECTORY ${lua_SOURCE_DIR}
    COMMENT "build lua"
)
add_custom_target(lua_lib DEPENDS ${lua_SOURCE_DIR}/src/liblua.a)

add_subdirectory(extern/SDL2)
add_subdirectory(extern/SDL2_ttf)
add_subdirectory(extern/SDL_image)

add_executable(cTester
    src/main.c
    src/gui_sdl.c
    src/gui_sdl_widget.c
)
target_include_directories(cTester PRIVATE ${lua_SOURCE_DIR}/src)
target_link_directories(cTester PRIVATE ${lua_SOURCE_DIR}/src)
target_link_libraries(cTester PRIVATE lua)
add_dependencies(cTester lua_lib)
target_link_libraries(cTester PRIVATE SDL2-static)
target_link_libraries(cTester PRIVATE SDL2main)
target_link_libraries(cTester PRIVATE SDL2_ttf)
target_link_libraries(cTester PRIVATE SDL2_image)
