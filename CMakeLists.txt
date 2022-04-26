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
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
)
FetchContent_Declare(
    objloader
    GIT_REPOSITORY https://github.com/Bly7/OBJ-Loader
)

FetchContent_MakeAvailable(FetchContentOffline glm SDL2 imgui objloader)

set(FETCHCONTENT_UPDATES_DISCONNECTED_FETCHCONTENTOFFLINE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${fetchcontentoffline_SOURCE_DIR}")
include(fetchcontent-offline)

FetchContent_DisconnectedIfOffline()


set(FETCHCONTENT_UPDATES_DISCONNECTED_SDL2 ON)


if(UNIX)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


add_library(imgui)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_sources(imgui PRIVATE
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui PRIVATE 
    SDL2
    ${CMAKE_DL_LIBS}
)


add_library(objloader INTERFACE)
target_include_directories(objloader INTERFACE
    ${objloader_SOURCE_DIR}/Source
)
target_sources(objloader INTERFACE
    ${objloader_SOURCE_DIR}/Source/OBJ_Loader.h
)

# ============================ LIBRARY ============================
add_library(ProfileExtruder)
target_include_directories(ProfileExtruder PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
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
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/camera.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/light.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/material.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/mesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/shader_program.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/utils/shader_program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
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
    imgui
    objloader
)
set_target_properties(ProfileExtruderDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/demo/
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/demo/
)