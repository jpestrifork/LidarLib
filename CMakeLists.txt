cmake_minimum_required(VERSION 3.28...4.0 )

project(cuLiDAR 
	VERSION 1.0
	DESCRIPTION "A demoproject for getting LiDAR data through cuda"
	LANGUAGES CXX CUDA
	)

find_package(fmt REQUIRED)
find_package(inno_client_sdk REQUIRED)
find_package(argparse REQUIRED)
find_package(PCL REQUIRED)
find_package(CUDAToolkit REQUIRED)

add_subdirectory(src)
