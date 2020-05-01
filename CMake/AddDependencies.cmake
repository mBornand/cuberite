# Set options for SQLiteCpp, disable all their tests and lints:
set(SQLITECPP_RUN_CPPLINT     OFF CACHE BOOL "Run cpplint.py tool for Google C++ StyleGuide."  FORCE)
set(SQLITECPP_RUN_CPPCHECK    OFF CACHE BOOL "Run cppcheck C++ static analysis tool."          FORCE)
set(SQLITECPP_RUN_DOXYGEN     OFF CACHE BOOL "Run Doxygen C++ documentation tool."             FORCE)
set(SQLITECPP_BUILD_EXAMPLES  OFF CACHE BOOL "Build examples."                                 FORCE)
set(SQLITECPP_BUILD_TESTS     OFF CACHE BOOL "Build and run tests."                            FORCE)
set(SQLITECPP_INTERNAL_SQLITE ON CACHE BOOL "Add the internal SQLite3 source to the project." FORCE)
set(SQLITE_ENABLE_COLUMN_METADATA OFF CACHE BOOL "" FORCE)

# Set options for LibEvent, disable all their tests and benchmarks:
set(EVENT__DISABLE_OPENSSL   YES CACHE BOOL   "Disable OpenSSL in LibEvent"       FORCE)
set(EVENT__DISABLE_BENCHMARK YES CACHE BOOL   "Disable LibEvent benchmarks"       FORCE)
set(EVENT__DISABLE_TESTS     YES CACHE BOOL   "Disable LibEvent tests"            FORCE)
set(EVENT__DISABLE_REGRESS   YES CACHE BOOL   "Disable LibEvent regression tests" FORCE)
set(EVENT__DISABLE_SAMPLES   YES CACHE BOOL   "Disable LibEvent samples"          FORCE)
set(EVENT__LIBRARY_TYPE "STATIC" CACHE STRING "Use static LibEvent libraries"     FORCE)

# Set options for JsonCPP, disabling all of their tests
# Additionally, their library is output to a strange location; make sure the linker knows where to find it
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Compile and (for jsoncpp_check) run JsonCpp test executables")
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Automatically run unit-tests as a post build step")
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "Generate and install .pc files")

# Set options for mbedtls
set(ENABLE_PROGRAMS OFF CACHE BOOL "Build mbed TLS programs.")
set(ENABLE_TESTING OFF CACHE BOOL "Build mbed TLS tests.")

# Check that the libraries are present:
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/SQLiteCpp/CMakeLists.txt)
	message(FATAL_ERROR "SQLiteCpp is missing in folder lib/SQLiteCpp. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/mbedtls/CMakeLists.txt)
	message(FATAL_ERROR "mbedTLS is missing in folder lib/mbedtls. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/libevent/CMakeLists.txt)
	message(FATAL_ERROR "LibEvent is missing in folder lib/libevent. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/jsoncpp/CMakeLists.txt)
	message(FATAL_ERROR "JsonCPP is missing in folder lib/jsoncpp. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/cmake-coverage/CodeCoverage.cmake)
	message(FATAL_ERROR "cmake-coverage is missing in folder lib/cmake-coverage. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/expat/CMakeLists.txt)
	message(FATAL_ERROR "expat is missing in folder lib/expat. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/fmt/CMakeLists.txt)
	message(FATAL_ERROR  "fmt is missing in folder lib/fmt. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/lua/CMakeLists.txt)
	message(FATAL_ERROR "lua is missing in folder lib/lua. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/luaexpat/CMakeLists.txt)
	message(FATAL_ERROR "luaexpat is missing in folder lib/luaexpat. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/luaproxy/CMakeLists.txt)
	message(FATAL_ERROR "luaproxy is missing in folder lib/luaproxy. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/lsqlite/CMakeLists.txt)
	message(FATAL_ERROR "lsqlite is missing in folder lib/lsqlite. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/tolua++/CMakeLists.txt)
	message(FATAL_ERROR "tolua++ is missing in folder lib/tolua++. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/zlib/CMakeLists.txt)
	message(FATAL_ERROR "zlib is missing in folder lib/zlib. Have you initialized and updated the submodules / downloaded the extra libraries?")
endif()

# Include all the libraries
# We use EXCLUDE_FROM_ALL so that only the explicit dependencies are used
# (mbedTLS also has test and example programs in their CMakeLists.txt, we don't want those)
add_subdirectory(lib/expat)
add_subdirectory(lib/fmt)
add_subdirectory(lib/jsoncpp)
add_subdirectory(lib/libevent EXCLUDE_FROM_ALL)
add_subdirectory(lib/lsqlite)
add_subdirectory(lib/lua)
add_subdirectory(lib/luaexpat)
add_subdirectory(lib/mbedtls)
add_subdirectory(lib/SQLiteCpp)
add_subdirectory(lib/tolua++ EXCLUDE_FROM_ALL)
add_subdirectory(lib/zlib)

# Add proper includes for LibEvent's event-config.h header:
target_include_directories(
	${CMAKE_PROJECT_NAME} SYSTEM
	PRIVATE lib/fmt/include
	PRIVATE lib/jsoncpp/include
	PRIVATE ${LIBEVENT_INCLUDE_DIRS}
	PRIVATE lib/lua/src
	PRIVATE lib/mbedtls/include
	PRIVATE lib/SQLiteCpp/include
	PRIVATE lib/TCLAP/include
	PRIVATE lib # TODO fix files including zlib/x instead of x
)

# Link dependencies as private
target_link_libraries(
	${CMAKE_PROJECT_NAME}
	PRIVATE event_core
	PRIVATE event_extra
	PRIVATE fmt::fmt
	PRIVATE jsoncpp_lib_static
	PRIVATE lua
	PRIVATE luaexpat
	PRIVATE mbedtls
	PRIVATE lsqlite
	PRIVATE SQLiteCpp
	PRIVATE tolualib
	PRIVATE zlib
)

# Special case handling for libevent pthreads
if(NOT WIN32)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE event_pthreads_static)
endif()

# Prettify jsoncpp_lib_static name in VS solution explorer
set_property(TARGET jsoncpp_lib_static PROPERTY PROJECT_LABEL "jsoncpp")

if (WIN32)
	add_subdirectory(lib/luaproxy/)
endif()
