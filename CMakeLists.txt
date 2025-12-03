# Minimum required version is 3.26 because of Stable Python ABI
cmake_minimum_required(VERSION 3.26...4.0 )

# Try to run conan before installing

if (SKBUILD)
execute_process(COMMAND conan install . )
endif ()

project(cuLiDAR 
	VERSION 1.0
	DESCRIPTION "A demoproject for getting LiDAR data through cuda"
	LANGUAGES CXX
	)

find_package(fmt REQUIRED)
find_package(inno_client_sdk REQUIRED)
find_package(argparse REQUIRED)
find_package(PCL REQUIRED)

add_subdirectory(src)
