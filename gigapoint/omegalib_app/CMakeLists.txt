cmake_minimum_required(VERSION 2.8.1) 
project(gigapoint)

find_package(Omegalib)
include_directories(${OMEGA_INCLUDE_DIRS})

SET( headers
        ../Utils.h
		../Shader.h
		../cJSON.h
		../NodeGeometry.h
		../PointCloud.h 
		../Thread.h
		../Menu.h 
		../ColorTexture.h 
		../Material.h
        )

SET( srcs 
		main.cpp
		../Utils.cpp
		../Shader.cpp
		../cJSON.cpp
		../NodeGeometry.cpp
		../PointCloud.cpp 
		../Thread.cpp 
		../Menu.cpp 
		../ColorTexture.cpp 
		../Material.cpp
		)

add_definitions(-DOMEGALIB_APP)

add_executable(gigapoint ${headers} ${srcs} )
target_link_libraries(gigapoint ${OMEGA_LIB} ${OMEGA_TOOLKIT_LIB} python2.7)

#file (COPY ${CMAKE_SOURCE_DIR}/../gigapoint_resource DESTINATION ${CMAKE_BINARY_DIR}/ )
