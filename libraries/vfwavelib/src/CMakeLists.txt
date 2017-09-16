set(VFWAVELIB_SRC_PATH "${PROJECT_SOURCE_DIR}/src/vfwavelib")
file(GLOB_RECURSE VFWAVELIB_SRC_FILES "*.c" "*.h" )

# external dependency
SET( Boost_USE_STATIC_LIBS OFF )
FIND_PACKAGE( Boost 1.58.0 
              COMPONENTS system log filesystem QUIET )

# Create 'vfwavelib' static lib
add_library( vfwavelib-static STATIC ${VFWAVELIB_SRC_FILES} )
set_target_properties( vfwavelib-static PROPERTIES LINKER_LANGUAGE CXX )
set_target_properties( vfwavelib-static PROPERTIES OUTPUT_NAME vfwavelib )
target_include_directories( vfwavelib-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} )
target_link_libraries( vfwavelib-static ${Boost_LIBRARIES} )
# Create 'vfwavelib' shared lib
add_library( vfwavelib-shared SHARED ${VFWAVELIB_SRC_FILES} )
set_target_properties( vfwavelib-shared PROPERTIES LINKER_LANGUAGE CXX )
set_target_properties( vfwavelib-shared PROPERTIES OUTPUT_NAME vfwavelib )
set_target_properties( vfwavelib-shared PROPERTIES SOVERSION ${VFWAVELIB_VERSION_MAJOR} )
set_target_properties( vfwavelib-shared PROPERTIES VERSION ${VFWAVELIB_VERSION} )
target_include_directories( vfwavelib-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} )
target_link_libraries( vfwavelib-shared ${Boost_LIBRARIES} )
SET_TARGET_PROPERTIES( vfwavelib-shared PROPERTIES MACOSX_BUNDLE TRUE )

### Install vfwavelib libs and include
INSTALL(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.tcc"
)

INSTALL(TARGETS vfwavelib-static DESTINATION lib/ OPTIONAL)
INSTALL(TARGETS vfwavelib-shared DESTINATION lib/ OPTIONAL)
