cmake_minimum_required(VERSION 3.5)
project(LowPolyEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Static Library Link to make .exe copyable under Windows
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

# Only necessary under Windows
include_directories("$ENV{VULKAN_SDK}/Include")

# Header only librarys (Should be precompiled)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "external/glm")

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "external/stb")


# Assets
file(GLOB_RECURSE shaders shaders/*)
file(COPY ${shaders} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/)

file(GLOB_RECURSE models models/*)
file(COPY ${models} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models/)


# Disable glfw Documentation, Tests and Examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#GLFW Build through CMake (Loads and links Vulkan as well)
add_subdirectory("external/glfw" glfw_local)

file(GLOB_RECURSE lpe_includes include/*)
file(GLOB_RECURSE lpe_sources src/*)
file(GLOB_RECURSE lpe_example_sources examples/*)
#Source Files Maybe use GLOB?
set(SOURCE_FILES ${lpe_sources} ${lpe_includes})

include_directories(include)
include_directories(test)

add_library(LowPolyEngine ${SOURCE_FILES})

# ${VULKAN_LIBRARY} gets definied by glfw
target_link_libraries(LowPolyEngine glfw ${VULKAN_LIBRARY} glm stb)

foreach(loop ${lpe_example_sources})
    get_filename_component(loopFileName ${loop} NAME)
    add_executable(LowPolyEngineExample_${loopFileName} ${loop})
    target_link_libraries(LowPolyEngineExample_${loopFileName} LowPolyEngine)
endforeach(loop)
