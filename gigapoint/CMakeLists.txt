module_version(0.9)

add_library(gigapoint MODULE 
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
	Menu.h
	Menu.cpp
	ColorTexture.h 
	ColorTexture.cpp 
	Material.h
	Material.cpp
    )
	
target_link_libraries(gigapoint omega)
declare_native_module(gigapoint)