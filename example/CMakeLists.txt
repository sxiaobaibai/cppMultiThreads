cmake_minimum_required(VERSION 3.8)
project("project-example")

set(CMAKE_CXX_FLAGS "-O3 -std=c++17 -Wall")


add_executable(example main.cc)

target_include_directories(example
	PRIVATE
	../inc/
)

target_link_libraries(example
	pthread
)
