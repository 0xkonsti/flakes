cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(flakes LANGUAGES C VERSION 0.1.0)

# CLANG_FORMAT ---------------------------------------------------------
find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_C_FILES *.c *.h)

    add_custom_target(format COMMAND ${CLANG_FORMAT} -i ${ALL_C_FILES})
endif()
#-----------------------------------------------------------------------

add_subdirectory(libs/utils)
add_subdirectory(libs/core)
add_subdirectory(apps/cli)