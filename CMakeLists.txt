cmake_minimum_required(VERSION 3.22)
project(ArtemisAlphonse)
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(glew-cmake EXCLUDE_FROM_ALL)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 opengl32 libglew_static)
