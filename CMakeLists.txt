cmake_minimum_required(VERSION 3.0)
project(Break_0_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Break_Infrastructure -Break_Graphics")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")


add_subdirectory(Infrastructure)
add_subdirectory(Graphics)

include_directories(Infrastructure/inc)
include_directories(Graphics/inc)

include_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glm/include")

link_directories("${CMAKE_CURRENT_LIST_DIR}/bin")

set(SOURCE_FILES main.cpp)
add_executable(Break_0_1 ${SOURCE_FILES} TestApplication.hpp)
target_link_libraries(Break_0_1 -lBreak_Infrastructure -lBreak_Graphics)