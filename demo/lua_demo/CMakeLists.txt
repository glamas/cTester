
project(lua_demo
    VERSION 0.0.1
    LANGUAGES C
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/api.c
)
target_include_directories(${PROJECT_NAME} PRIVATE ${lua_SOURCE_DIR}/src)
target_link_directories(${PROJECT_NAME} PRIVATE ${lua_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE lua)
add_dependencies(${PROJECT_NAME} lua_lib)


# set(lib_name "api")
# add_library(${lib_name} SHARED ${PROJECT_SOURCE_DIR}/api.c)
# target_include_directories(${lib_name} PRIVATE ${lua_SOURCE_DIR}/src)
# target_link_directories(${lib_name} PRIVATE ${lua_SOURCE_DIR}/src)
# target_link_libraries(${lib_name} PRIVATE lua)
# set_target_properties(${lib_name} PROPERTIES
#     LINKER_LANGUAGE C
#     ENABLE_EXPORTS ON
#     PREFIX ""
#     OUTPUT_NAME ${lib_name})
