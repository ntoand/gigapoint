cmake_minimum_required(VERSION 2.8.1) 
project(opotree)

find_package(Omegalib)

include_directories(${OMEGA_INCLUDE_DIRS})
add_executable(opotree main.cpp Utils.cpp Shader.cpp MyTriangle.cpp)
target_link_libraries(opotree ${OMEGA_LIB})