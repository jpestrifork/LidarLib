add_executable(cuLiDAR)
add_library(processor SHARED)

target_sources(cuLiDAR
	PRIVATE
		main.cpp
	PRIVATE
		FILE_SET HEADERS
)

target_link_libraries(cuLiDAR PRIVATE argparse::argparse fmt::fmt processor)


target_sources( processor
	PRIVATE
		Processor.cpp
	PUBLIC
		FILE_SET processorPublicHeaders
		TYPE HEADERS
		BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		FILE_SET HEADERS
)
target_link_libraries(processor
	PRIVATE
		inno_client_sdk::inno_client_sdk
		fmt::fmt
	PUBLIC
		PCL::PCL)

set(Python_FIND_UNVERSIONED_NAMES FIRST)
find_package(Python REQUIRED COMPONENTS Interpreter Development.SABIModule Development.Module )
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(inno STABLE_ABI NB_STATIC module.cpp)
target_sources( inno
	PRIVATE
		Lidar.cpp
	PUBLIC
		FILE_SET innoPublicHeaders
		TYPE HEADERS
		BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		FILE_SET HEADERS
)
target_link_libraries(inno PRIVATE processor)
set_target_properties(inno PROPERTIES CMAKE_BUILD_TYPE "Release")