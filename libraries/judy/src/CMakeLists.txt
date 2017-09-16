set(JUDY_SRC_PATH "${PROJECT_SOURCE_DIR}/src/judy")
file(GLOB_RECURSE JUDY_SRC_FILES "*.cpp" "*.c" "*.hpp" )

# external dependency
SET( Boost_USE_STATIC_LIBS OFF )
FIND_PACKAGE( Boost COMPONENTS system log filesystem QUIET )

# Create 'judy' static lib
add_library( judy-static STATIC ${JUDY_SRC_FILES} )
set_target_properties( judy-static PROPERTIES LINKER_LANGUAGE CXX )
set_target_properties( judy-static PROPERTIES OUTPUT_NAME judy )
target_include_directories( judy-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} )
target_link_libraries( judy-static ${Boost_LIBRARIES} )
# Create 'judy' shared lib
add_library( judy-shared SHARED ${JUDY_SRC_FILES} )
set_target_properties( judy-shared PROPERTIES LINKER_LANGUAGE CXX )
set_target_properties( judy-shared PROPERTIES OUTPUT_NAME judy )
set_target_properties( judy-shared PROPERTIES SOVERSION ${JUDY_VERSION_MAJOR} )
set_target_properties( judy-shared PROPERTIES VERSION ${JUDY_VERSION} )
target_include_directories( judy-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( judy-shared ${Boost_LIBRARIES} )
SET_TARGET_PROPERTIES( judy-shared PROPERTIES MACOSX_BUNDLE TRUE )

### Install judy libs and include
INSTALL(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.tcc"
)

INSTALL(TARGETS judy-static DESTINATION lib/ OPTIONAL)
INSTALL(TARGETS judy-shared DESTINATION lib/ OPTIONAL)
