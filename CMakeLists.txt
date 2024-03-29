#
# top-level CMake configuration file for PDAL
#
# (based originally on the libLAS files copyright Mateusz Loskot)

cmake_minimum_required(VERSION 2.8.4)
project(spatialindex)



set (CMAKE_CXX_FLAGS "$ENV{CMAKE_CXX_FLAGS} -std=c++14 -O3")
set (CMAKE_CXX_FLAGS_DEBUG  "$ENV{CMAKE_CXX_FLAGS}  -g -O0")
#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------
if(NOT WIN32)
    #stxxl
    add_subdirectory(stxxl)
    include_directories(${STXXL_INCLUDE_DIRS})
endif()



set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

cmake_policy(SET CMP0054 NEW) # Make string comparison behave like you'd expect

if (CMAKE_MAJOR_VERSION GREATER 2)
    cmake_policy(SET CMP0042 OLD) # osx rpath
endif()
#------------------------------------------------------------------------------
# libspatialindex general settings
#------------------------------------------------------------------------------

SET(SIDX_VERSION_MAJOR "1")
SET(SIDX_VERSION_MINOR "9")
SET(SIDX_VERSION_PATCH "0")
SET(SIDX_LIB_VERSION "5.0.0")
SET(SIDX_LIB_SOVERSION "5")

set(SIDX_VERSION_STRING "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_PATCH}")

#------------------------------------------------------------------------------
# libspatialindex general cmake options
#------------------------------------------------------------------------------

option(SIDX_BUILD_TESTS "Enables integrated test suites" ON)

if (MINGW)
    add_definitions(-DWIN32)
endif()

# Name of C++ library

set(SIDX_LIB_NAME spatialindex)
set(SIDX_C_LIB_NAME spatialindex_c)

if(WIN32)
    if (MSVC)
        if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set( SIDX_LIB_NAME "spatialindex-64" )
            set( SIDX_C_LIB_NAME "spatialindex_c-64" )
        else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set( SIDX_LIB_NAME "spatialindex-32"  )
            set( SIDX_C_LIB_NAME "spatialindex_c-32"  )
        endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    endif()
endif()


set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

include (CheckFunctionExists)

check_function_exists(srand48 HAVE_SRAND48)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(memcpy HAVE_MEMCPY)
check_function_exists(bcopy HAVE_BCOPY)


INCLUDE (CheckIncludeFiles)

if (HAVE_SRAND48)
    add_definitions(-DHAVE_SRAND48=1)
endif()

if (HAVE_GETTIMEOFDAY)
    add_definitions(-DHAVE_GETTIMEOFDAY=1)
endif()

if (HAVE_BZERO)
    add_definitions(-DHAVE_BZERO=1)
endif()

if (HAVE_MEMSET)
    add_definitions(-DHAVE_MEMSET=1)
endif()

if (HAVE_MEMCPY)
    add_definitions(-DHAVE_MEMCPY=1)
endif()

if (HAVE_BCOPY)
    add_definitions(-DHAVE_BCOPY=1)
endif()

if (HAVE_PTHREAD_H)
    add_definitions(-DHAVE_PTHREAD_H=1)
endif()

#------------------------------------------------------------------------------
# General build settings
#------------------------------------------------------------------------------

# note we default to debug mode
if(NOT MSVC_IDE)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
                "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
    endif()
    message(STATUS "Setting libspatialindex build type - ${CMAKE_BUILD_TYPE}")
endif()

set(SIDX_BUILD_TYPE ${CMAKE_BUILD_TYPE})

option(ASAN "build with asan" OFF)

if(ASAN)
    SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} $ENV{CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
    #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lasan")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} $ENV{CMAKE_CXX_FLAGS} -g -O3 -DNDEBUG")
# TODO: Still testing the output paths --mloskot
set(SIDX_BUILD_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})


#------------------------------------------------------------------------------
# Platform and compiler specific settings
#------------------------------------------------------------------------------

if(WIN32)
    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-D_CRT_NONSTDC_NO_WARNING)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        add_definitions(-DSPATIALINDEX_CREATE_DLL=1)
        add_definitions(-DNOMINMAX)
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4068")

        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4"
                    CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()
    endif()

else()
    # not WIN32

    # Recommended C++ compilation flags
    # -Weffc++
    set(SIDX_COMMON_CXX_FLAGS
            "-pedantic -Wall -Wpointer-arith -Wcast-align -Wcast-qual  -Wredundant-decls -Wno-long-long")

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SIDX_COMMON_CXX_FLAGS}")
        if (CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_SYSTEM_NAME STREQUAL "CYGWIN")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
        endif()

        set(SIDX_COMPILER_GCC 1)

    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
        set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS} ${SIDX_COMMON_CXX_FLAGS}")
        set(SIDX_COMPILER_CLANG 1)
    endif()
endif(WIN32)

if (APPLE)
    set(SO_EXT dylib)
    set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
    set(SO_EXT dll)
else()
    set(SO_EXT so)
endif(APPLE)


enable_testing()

#------------------------------------------------------------------------------
# installation path settings
#------------------------------------------------------------------------------

if(WIN32)
    set(DEFAULT_LIB_SUBDIR lib)
    set(DEFAULT_DATA_SUBDIR .)
    set(DEFAULT_INCLUDE_SUBDIR include)

    if (MSVC)
        set(DEFAULT_BIN_SUBDIR bin)
    else()
        set(DEFAULT_BIN_SUBDIR .)
    endif()
else()
    # Common locations for Unix and Mac OS X
    set(DEFAULT_BIN_SUBDIR bin)
    set(DEFAULT_LIB_SUBDIR lib${LIB_SUFFIX})
    set(DEFAULT_DATA_SUBDIR share/spatialindex)
    set(DEFAULT_INCLUDE_SUBDIR include)
endif()

# Locations are changeable by user to customize layout of SIDX installation
# (default values are platform-specific)
set(SIDX_BIN_SUBDIR ${DEFAULT_BIN_SUBDIR} CACHE STRING
        "Subdirectory where executables will be installed")
set(SIDX_LIB_SUBDIR ${DEFAULT_LIB_SUBDIR} CACHE STRING
        "Subdirectory where libraries will be installed")
set(SIDX_INCLUDE_SUBDIR ${DEFAULT_INCLUDE_SUBDIR} CACHE STRING
        "Subdirectory where header files will be installed")
set(SIDX_DATA_SUBDIR ${DEFAULT_DATA_SUBDIR} CACHE STRING
        "Subdirectory where data will be installed")

# Mark *_SUBDIR variables as advanced and dedicated to use by power-users only.
mark_as_advanced(SIDX_BIN_SUBDIR
        SIDX_LIB_SUBDIR SIDX_INCLUDE_SUBDIR SIDX_DATA_SUBDIR)

# Full paths for the installation
set(SIDX_BIN_DIR ${SIDX_BIN_SUBDIR})
set(SIDX_LIB_DIR ${SIDX_LIB_SUBDIR})
set(SIDX_INCLUDE_DIR ${SIDX_INCLUDE_SUBDIR})
set(SIDX_DATA_DIR ${SIDX_DATA_SUBDIR})

#------------------------------------------------------------------------------
# subdirectory controls
#------------------------------------------------------------------------------

add_subdirectory(src)

add_subdirectory(sqlite)
include_directories(sqlite)

if(SIDX_BUILD_TESTS)
    add_subdirectory(test)
endif()


#------------------------------------------------------------------------------
# CPACK controls
#------------------------------------------------------------------------------

SET(CPACK_PACKAGE_VERSION_MAJOR ${PDAL_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PDAL_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PDAL_VERSION_MINOR})
SET(CPACK_PACKAGE_NAME "libspatialindex")

SET(CPACK_SOURCE_GENERATOR "TBZ2;TGZ")
SET(CPACK_PACKAGE_VENDOR "libspatialindex Development Team")
SET(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/COPYING")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
        "${CMAKE_PROJECT_NAME}-${SIDX_VERSION_STRING}-src")

set(CPACK_SOURCE_IGNORE_FILES
        "/\\\\.gitattributes;/\\\\.vagrant;/\\\\.DS_Store;/CVS/;/\\\\.git/;\\\\.swp$;~$;\\\\.\\\\#;/\\\\#")

list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeScripts/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeCache.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".xcodeproj")
list(APPEND CPACK_SOURCE_IGNORE_FILES "build.make")
list(APPEND CPACK_SOURCE_IGNORE_FILES "_CPack_Packages")
list(APPEND CPACK_SOURCE_IGNORE_FILES "cmake_install.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "Testing")
list(APPEND CPACK_SOURCE_IGNORE_FILES "PDAL.build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/bin/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/lib/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "Makefile")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeFiles")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CTestTestfile.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/test/data/local/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/test/unit/TestConfig.hpp$")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/doc/doxygen/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/doc/build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/doc/presentations/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/doc/_static/logo/dongle/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/cmake/examples/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "pdal_features.hpp$")
list(APPEND CPACK_SOURCE_IGNORE_FILES "package.sh")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".gz2")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".bz2")

include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)


