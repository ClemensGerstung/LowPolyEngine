#someone who known how to write proper cmake files is welcome to refactor this...

cmake_minimum_required(VERSION 3.5)
project(LowPolyEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Static Library Link to make .exe copyable under Windows
if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

# Only necessary under Windows
include_directories("$ENV{VULKAN_SDK}/include")

# Header only librarys (Should be precompiled)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "external/glm")

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "external/stb")

# Assets
file(GLOB_RECURSE shaders assets/shaders/*.spv)
file(COPY ${shaders} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/)

file(GLOB_RECURSE models assets/models/*)
file(COPY ${models} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models/)

file(GLOB_RECURSE textures assets/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures/)


# Disable glfw Documentation, Tests and Examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#GLFW Build through CMake (Loads and links Vulkan as well)
add_subdirectory("external/glfw" glfw_local)

file(GLOB_RECURSE lpe_sources src/*)
file(GLOB lpe_example_sources examples/*)
#Source Files Maybe use GLOB?
set(SOURCE_FILES ${lpe_sources} include/lpe/lpe.h)

include_directories(test)

add_library(LowPolyEngine ${SOURCE_FILES})

# ${VULKAN_LIBRARY} gets definied by glfw
target_link_libraries(LowPolyEngine glfw ${VULKAN_LIBRARY} glm stb)

#if(NOT LPE_DISABLE_EXAMPLES)
  foreach(folder ${lpe_example_sources})
    if(IS_DIRECTORY ${folder})
      get_filename_component(example_name ${folder} NAME)

      file(GLOB_RECURSE example_source ${folder}/*)

      include_directories(include)

      add_executable(LPExample_${example_name} ${example_source})
      target_link_libraries(LPExample_${example_name} LowPolyEngine)
    endif()
  endforeach(folder)
#endif()

if(NOT LPE_DISABLE_TESTS)
  #enable_testing()

  add_subdirectory("external/googletest" gtest_local)
  file(GLOB lpe_tests tests/*)

  add_executable(LPE_Test ${lpe_tests})
  target_link_libraries(LPE_Test LowPolyEngine gtest gtest_main)
endif()