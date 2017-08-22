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
include_directories("external/glm")
include_directories("external/stb")
include_directories("external/tinyobjloader")

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
target_link_libraries(LowPolyEngine glfw ${VULKAN_LIBRARY})

