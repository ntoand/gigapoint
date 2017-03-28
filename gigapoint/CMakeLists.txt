#module_version(1.0.0)
#declare_native_module(gigapoint)

cmake_minimum_required(VERSION 2.8.1) 
find_package(Omegalib)

# Set the module name here
SET(MODULE_NAME gigapoint)

include_directories(${OMEGA_INCLUDE_DIRS})

set( FEATURE_ONLINEUPDATE_ENABLED FALSE CACHE BOOL "Enable modifications of the Pointcloud files at runtime." )
if( FEATURE_ONLINEUPDATE_ENABLED )
  add_definitions(-DONLINEUPDATE)
endif()

set( INTERACTION_ENABLED TRUE CACHE BOOL "Enable Interaction with the Pointcloud." )
if( INTERACTION_ENABLED )
  add_definitions(-DINTERACTION)
endif()


# Set module name and source files here
add_library(${MODULE_NAME} MODULE 
	gigapoint.cpp
	Utils.h
	Utils.cpp
	Shader.h
	Shader.cpp
	cJSON.h
	cJSON.cpp
	NodeGeometry.h
	NodeGeometry.cpp
	PointCloud.h
	PointCloud.cpp 
	Thread.h
	Thread.cpp 
	ColorTexture.h 
	ColorTexture.cpp 
	Material.h
	Material.cpp
        FractureTracer.cpp
        FractureTracer.h
	FrameBuffer.h
	FrameBuffer.cpp
        Interaction.h
        Interaction.cpp
        LRU.h        
        )

# Set the module library dependencies here
target_link_libraries(${MODULE_NAME}
        stdc++
        ${OMEGA_LIB}
        python2.7
        )

#------------------------------------------------------------------------------
# DO NOT MODIFY ANYTHING BELOW AFTER THIS LINE
set_target_properties(${MODULE_NAME} PROPERTIES PREFIX "")
if(WIN32)
        set_target_properties(${MODULE_NAME} PROPERTIES FOLDER modules SUFFIX ".pyd")
endif()

