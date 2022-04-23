cmake_minimum_required(VERSION 3.0.0)
project(ProfileExtruder VERSION 1.0.0)

include(FetchContent)

# ============================ DEPENDENCIES ============================
FetchContent_Declare(
    FetchContentOffline
    GIT_REPOSITORY https://github.com/SpontanCombust/cmake-fetchcontent-offline
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG main
)

FetchContent_MakeAvailable(FetchContentOffline glm SDL2)

set(FETCHCONTENT_UPDATES_DISCONNECTED_FETCHCONTENTOFFLINE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${fetchcontentoffline_SOURCE_DIR}")
include(fetchcontent-offline)

if(UNIX)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


# ============================ LIBRARY ============================
add_library(ProfileExtruder)
target_include_directories(ProfileExtruder PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_sources(ProfileExtruder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/bezier_curve.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bezier_curve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/curve_mesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/curve_mesh.cpp
)
target_link_libraries(ProfileExtruder PRIVATE
    glm
)

# ============================ DEMO ============================
add_executable(ProfileExtruderDemo)
target_sources(ProfileExtruderDemo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/shader_program.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/shader_program.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
)
target_link_libraries(ProfileExtruderDemo PRIVATE
    ProfileExtruder
    SDL2
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)
