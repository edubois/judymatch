# Project judymatch
cmake_minimum_required (VERSION 2.8)
project (JudyMatch)

# Boost specific
ADD_DEFINITIONS( -DBOOST_LOG_DYN_LINK )
ADD_DEFINITIONS( -DBOOST_TEST_DYN_LINK )

# Add a variable for production
add_definitions(-DJUDYMATCH_PRODUCTION)

set(JUDYMATCH_PRODUCTION TRUE CACHE BOOL "Fully optimized for production usage.")
if(JUDYMATCH_PRODUCTION)
  add_definitions(-DJUDYMATCH_PRODUCTION)
endif()

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

if(CMAKE_COMPILER_IS_GNUCXX)
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    endif()
endif()

if(APPLE)
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    SET(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
    SET(CMAKE_MACOSX_RPATH 0)
endif()

#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -stdlib=libc++")

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    set(TARGET_ARCHITECTURE "x86")
endif()

# The install rule does not depend on all, i.e. everything will not be built before installing
set( CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE )

# Include subdirs
add_subdirectory(libraries)
add_subdirectory(applications)
add_subdirectory(tests)
