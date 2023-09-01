cmake_minimum_required(VERSION 3.14..3.22)


project(cTester
    VERSION 0.0.1
    LANGUAGES C
)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

## main
add_subdirectory(src)

## demo
