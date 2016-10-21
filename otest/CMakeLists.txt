cmake_minimum_required(VERSION 2.8.1) 
project(otest)

find_package(Omegalib)
include_directories(${OMEGA_INCLUDE_DIRS})
add_executable(otest Points.cpp Shader.cpp Utils.cpp main.cpp)
target_link_libraries(otest ${OMEGA_LIB})