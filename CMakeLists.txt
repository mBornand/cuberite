# This is the top-level CMakeLists.txt file for the Cuberite project
#
# Use CMake to generate the build files for your platform
#
# This script supports some configuration through CMake arguments (-Dparam=val syntax):
#   BUILD_TOOLS=1            sets up additional executables to be built along with the server (ProtoProxy, GrownBiomeGenVisualiser, MCADefrag)
#   BUILD_UNSTABLE_TOOLS=1   sets up yet more executables to be built, these can be broken and generally are obsolete (GeneratorPerformanceTest)
#   NO_NATIVE_OPTIMIZATION=1 disables CPU-specific optimisations for the current machine, allows use on other CPUs of the same platform
#   DISABLE_SYSTEM_LUA=1     disables the use of system Lua interpreter; the tolua executable will be built and used instead. Incompatible with cross-compiling
#   SELF_TEST=1              enables testing code to be built





cmake_minimum_required (VERSION 3.16)
project(Cuberite C CXX)

set(BUILD_TOOLS OFF CACHE BOOL "")
set(SELF_TEST OFF CACHE BOOL "")

# We need C++17 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The need for speed (in Release)
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

# Static CRT
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include("CMake/StampBuild.cmake")
add_executable(${CMAKE_PROJECT_NAME})
add_subdirectory(src)

# Set the startup project to Cuberite, and the debugger dir:
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Server")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

# Self Test Mode enables extra checks at startup
if(${SELF_TEST})
	enable_testing()
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE SELF_TEST)
	add_subdirectory(tests)
endif()

include("CMake/AddDependencies.cmake")
include("CMake/ConditionallyBuildTools.cmake")
include("CMake/Fixups.cmake")
include("CMake/GenerateBindings.cmake")
include("CMake/GroupSources.cmake")
include("CMake/MSVCFlags.cmake")
