# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24)

project ("NativeNamedPipe")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include sub-projects.
add_subdirectory(fmt)
add_subdirectory("UnityNamedPipe.Native")
add_subdirectory(NativeTest)
