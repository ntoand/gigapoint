cmake_minimum_required(VERSION 2.8.1) 
project(opotree)

find_package(Omegalib)
include_directories(${OMEGA_INCLUDE_DIRS})

SET( headers
        
        )

SET( srcs 
		main.cpp
		Utils.cpp
		Shader.cpp
		cJSON.cpp
		NodeGeometry.cpp
		PointCloud.cpp 
		Thread.cpp 
		Menu.cpp 
		ColorTexture.cpp 
		Material.cpp
		)

add_executable(opotree ${headers} ${srcs} )
target_link_libraries(opotree ${OMEGA_LIB} ${OMEGA_TOOLKIT_LIB} python2.7)

file (COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/ )
file (COPY ${CMAKE_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR}/ )
file (COPY ${CMAKE_SOURCE_DIR}/data/colortable.dat DESTINATION ${CMAKE_BINARY_DIR}/ )