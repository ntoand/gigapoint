cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
#cmake_minimum_required(VERSION 2.8.1)
project(opotree)

set(NO_GRAPHICS OFF)



IF(${NO_GRAPHICS})
	add_definitions(-DNO_GRAPHICS)
	add_executable(opotree main_test.cpp cJSON.cpp Utils.cpp)
	target_link_libraries(opotree )

ELSE()
	find_package(Omegalib)
	include_directories(${OMEGA_INCLUDE_DIRS})
	add_executable(opotree main.cpp Utils.cpp Shader.cpp cJSON.cpp NodeGeometry.cpp PointCloud.cpp Thread.cpp Menu.cpp ColorTexture.cpp Material.cpp FractureTracer.cpp)
	target_link_libraries(opotree ${OMEGA_LIB} ${OMEGA_TOOLKIT_LIB} python2.7)
        target_compile_features(opotree PRIVATE cxx_range_for)

ENDIF()

