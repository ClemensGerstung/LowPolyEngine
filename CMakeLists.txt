cmake_minimum_required(VERSION 3.8)
project(LowPolyEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)

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

add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE "external/tinyobjloader")

# Assets
file(GLOB_RECURSE shaders LowPolyEngine.Test/shaders/*)
file(COPY ${shaders} DESTINATION shaders/)

file(GLOB_RECURSE textures LowPolyEngine.Test/textures/*)
file(COPY ${textures} DESTINATION textures/)

file(GLOB_RECURSE models LowPolyEngine.Test/models/*)
file(COPY ${models} DESTINATION models/)


# Disable glfw Documentation, Tests and Examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#GLFW Build through CMake (Loads and links Vulkan as well)
add_subdirectory("external/glfw" glfw_local)

file(GLOB_RECURSE lpe_sources LowPolyEngine/*)
file(GLOB_RECURSE lpe_test_sources LowPolyEngine.Test/*)
#Source Files Maybe use GLOB?
set(SOURCE_FILES ${lpe_sources} ${lpe_test_sources})

include_directories(.)
include_directories(LowPolyEngine)
include_directories(LowPolyEngine.Test)

add_executable(LowPolyEngine ${SOURCE_FILES})

# ${VULKAN_LIBRARY} gets definied by glfw
target_link_libraries(LowPolyEngine glfw ${VULKAN_LIBRARY} glm stb tinyobjloader)

