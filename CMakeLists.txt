cmake_minimum_required(VERSION 3.8)
project(LowPolyEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 11)

include_directories("$ENV{VULKAN_SDK}/Include")
include_directories("external/glm")
include_directories("external/stb")
include_directories("external/tinyobjloader")

link_directories("$ENV{VULKAN_SDK}/Source/lib")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#GLFW Build through CMake
add_subdirectory("external/glfw" glfw_local)

set(SOURCE_FILES
        LowPolyEngine/Window.cpp
        LowPolyEngine/Window.h
        LowPolyEngine.Test/Main.cpp
)

include_directories(.)
include_directories(LowPolyEngine)
include_directories(LowPolyEngine.Test)

add_executable(LowPolyEngine ${SOURCE_FILES})


if (WIN32)
    target_link_libraries(LowPolyEngine glfw vulkan-1)
else()
    target_link_libraries(LowPolyEngine glfw ${VULKAN_LIBRARY})
endif()
