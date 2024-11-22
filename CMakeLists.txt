cmake_minimum_required (VERSION 3.16)
cmake_policy(GET CMP0135 NEW)
project(Fluorite2 VERSION 0.0.1)

include("cmake/macros.cmake")
include(FetchContent)

MACRO_ENSURE_OUT_OF_SOURCE_BUILD("${CMAKE_PROJECT_NAME} requires an out of source build.")


FetchContent_Declare(ogre3d URL https://github.com/OGRECave/ogre/archive/refs/tags/v14.3.1.zip)
FetchContent_MakeAvailable(ogre3d)
find_package(SDL2 REQUIRED)


file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(app ${SOURCES})
set_property(TARGET app PROPERTY CXX_STANDARD 20)


target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_include_directories(app PRIVATE ${ogre3d_BINARY_DIR}/sdk/include/OGRE)
target_include_directories(app PRIVATE ${ogre3d_BINARY_DIR}/sdk/include/OGRE/RenderSystems/glad)
target_include_directories(app PRIVATE ${ogre3d_BINARY_DIR}/sdk/include/OGRE/RenderSystems/GLES2)
target_link_libraries(app SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(app OgreMain OgreGLSupport RenderSystem_GL)

add_custom_command(
  TARGET app POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:app>
  VERBATIM
)

add_custom_command(
  TARGET app POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:OgreMain>
    $<TARGET_FILE_DIR:app>
  VERBATIM
)

add_custom_command(
  TARGET app POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:RenderSystem_GL>
    $<TARGET_FILE_DIR:app>
  VERBATIM
)