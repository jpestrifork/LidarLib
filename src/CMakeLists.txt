add_executable(cuLiDAR)
add_library(processor)

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
	PUBLIC
		PCL::PCL)
