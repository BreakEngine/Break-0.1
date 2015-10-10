cmake_minimum_required(VERSION 3.0)
project(Break_Infrastructure)

if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mwindows -lglew32 -lglfw3")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lglew32 -lglfw3")
endif(WIN32)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib")

file(GLOB HEADERS inc/*.hpp)
file(GLOB SOURCES src/*.cpp)

file(GLOB OUTPUT "lib/*.*")

include_directories(inc)


include_directories("deps/glm/include")
include_directories("deps/glew-1.10.0/include")
include_directories("deps/glfw-3.1.bin.WIN32/include")

if(MSVC)
link_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glew-1.10.0/lib/Release/Win32")
if(MSVC_VERSION EQUAL 1600)
link_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.1.bin.WIN32/lib-vs2010")
elseif(MSVC_VERSION EQUAL 1700)
link_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.1.bin.WIN32/lib-vs2012")
elseif(MSVC_VERSION EQUAL 1800)
link_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.1.bin.WIN32/lib-vs2013")
endif()
elseif(MINGW)
link_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glew-1.10.0/lib-mingw")
link_directories("${CMAKE_CURRENT_LIST_DIR}/deps/glfw-3.1.bin.WIN32/lib-mingw")
else()
message(WARNING "you have to provide dependencies since we can't identify your toolchain")
endif(MSVC)

if(WIN32)
link_directories("$ENV{DXSDK_DIR}/Lib/x64")
endif(WIN32)


add_library(Break_Infrastructure SHARED ${HEADERS} ${SOURCES})

if(WIN32)
target_link_libraries(Break_Infrastructure d3d11 d3dx11_43 gdi32 dxgi d3dcompiler glew32 glfw3 OPENGL32)
endif(WIN32)

foreach(output_file ${OUTPUT})
add_custom_command(TARGET Break_Infrastructure POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "${output_file}" "${CMAKE_CURRENT_LIST_DIR}/../bin")
endforeach(output_file)