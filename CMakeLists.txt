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

FetchContent_MakeAvailable(FetchContentOffline glm)

set(FETCHCONTENT_UPDATES_DISCONNECTED_FETCHCONTENTOFFLINE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${fetchcontentoffline_SOURCE_DIR}")
include(fetchcontent-offline)


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
)
target_link_libraries(ProfileExtruderDemo PRIVATE
    ProfileExtruder
)