cmake_minimum_required(VERSION 3.0)
project(Break_Graphics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Break_Infrastructure")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib")

file(GLOB HEADERS inc/*.hpp)
file(GLOB SOURCES src/*.cpp)



include_directories("../Infrastructure/inc")

include_directories("deps/glm/include/")

include_directories(inc)

link_directories("../Infrastructure/lib")

add_library(Break_Graphics SHARED ${HEADERS} ${SOURCES})
target_link_libraries(Break_Graphics -lBreak_Infrastructure)

file(GLOB OUTPUT "lib/*.dll")

foreach(output_file ${OUTPUT})
add_custom_command(TARGET Break_Graphics POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "${output_file}" "${CMAKE_CURRENT_LIST_DIR}/../bin/")
endforeach(output_file)