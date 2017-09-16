set(JUDYMATCH_SRC_PATH "${PROJECT_SOURCE_DIR}/src/judymatch")
file(GLOB_RECURSE JUDYMATCH_SRC_FILES "*.cpp" "*.hpp" )

# external dependency
SET( Boost_USE_STATIC_LIBS OFF )
FIND_PACKAGE( Boost COMPONENTS system log filesystem QUIET )

# Create 'judymatch' static lib
add_library( judymatch-static STATIC ${JUDYMATCH_SRC_FILES} )
set_target_properties( judymatch-static PROPERTIES LINKER_LANGUAGE CXX )
set_target_properties( judymatch-static PROPERTIES OUTPUT_NAME judymatch )
set_property(TARGET judymatch-static PROPERTY CXX_STANDARD 14)
target_include_directories( judymatch-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} judy-static vfwavelib-static )
target_link_libraries( judymatch-static ${Boost_LIBRARIES} judy-static vfwavelib-static )

# Create 'judymatch' shared lib
add_library( judymatch-shared SHARED ${JUDYMATCH_SRC_FILES} )
set_target_properties( judymatch-shared PROPERTIES LINKER_LANGUAGE CXX )
set_target_properties( judymatch-shared PROPERTIES OUTPUT_NAME judymatch )
set_target_properties( judymatch-shared PROPERTIES SOVERSION ${JUDYMATCH_VERSION_MAJOR} )
set_target_properties( judymatch-shared PROPERTIES VERSION ${JUDYMATCH_VERSION} )
set_property(TARGET judymatch-shared PROPERTY CXX_STANDARD 14)
target_include_directories( judymatch-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} judy-shared vfwavelib-shared )
target_link_libraries( judymatch-shared ${Boost_LIBRARIES} judy-shared vfwavelib-shared )
SET_TARGET_PROPERTIES( judymatch-shared PROPERTIES MACOSX_BUNDLE TRUE )

### Install judymatch libs and include
INSTALL(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.tcc"
)

INSTALL(TARGETS judymatch-static DESTINATION lib/ OPTIONAL)
INSTALL(TARGETS judymatch-shared DESTINATION lib/ OPTIONAL)
