cmake_minimum_required (VERSION 3.16)
project(Fluorite2 VERSION 0.0.1)

include("cmake/macros.cmake")
include(FetchContent)

MACRO_ENSURE_OUT_OF_SOURCE_BUILD("${CMAKE_PROJECT_NAME} requires an out of source build.")


FetchContent_Declare(ogre3d URL https://github.com/OGRECave/ogre/archive/refs/tags/v14.3.1.zip)
FetchContent_MakeAvailable(ogre3d)
find_package(SDL2 REQUIRED)


file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(app ${SOURCES})

message(${SOURCES})

target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/src)


target_include_directories(app PRIVATE ${ogre3d_SOURCE_DIR}/OgreMain/Include)
target_include_directories(app PRIVATE ${ogre3d_BINARY_DIR}/include)
target_link_libraries(app SDL2::SDL2 SDL2::SDL2main)

add_custom_command(
  TARGET app POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:app>
  VERBATIM
)