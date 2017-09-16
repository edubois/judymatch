cmake_minimum_required(VERSION 2.8.11)

project(Judy CXX)

# Set sequenceParser versions
set(JUDY_VERSION_MAJOR "1")
set(JUDY_VERSION_MINOR "0")
set(JUDY_VERSION_MICRO "0")
set(JUDY_VERSION ${JUDY_VERSION_MAJOR}.${JUDY_VERSION_MINOR}.${JUDY_VERSION_MICRO})

# Define sequenceParser versions
add_definitions(-DJUDY_VERSION_MAJOR=${JUDY_VERSION_MAJOR})
add_definitions(-DJUDY_VERSION_MINOR=${JUDY_VERSION_MINOR})
add_definitions(-DJUDY_VERSION_MICRO=${JUDY_VERSION_MICRO})

# CPP flags on debug / release mode
if(MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -pg -g")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")
endif()

ADD_SUBDIRECTORY(src)
