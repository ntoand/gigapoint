cmake_minimum_required(VERSION 2.8.1) 
project(opotree)

set(LOCAL_TEST OFF)

IF(${LOCAL_TEST})
	add_executable(opotree main_test.cpp cJSON.cpp PCLoader.cpp NodeGeometry.cpp)
	target_link_libraries(opotree )

ELSE()
	find_package(Omegalib)
	include_directories(${OMEGA_INCLUDE_DIRS})
	add_executable(opotree main.cpp Utils.cpp Shader.cpp cJSON.cpp PCLoader.cpp NodeGeometry.cpp Material.cpp PointCloud.cpp)
	target_link_libraries(opotree ${OMEGA_LIB})

ENDIF()

