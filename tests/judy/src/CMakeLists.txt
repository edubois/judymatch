SET( MY_APP_NAME "TestJudy" )

# external modules
FILE( GLOB_RECURSE TEST_JUDY_SRCS "*.cpp" "*.hpp" "*.tcc" )
FIND_PACKAGE( Boost COMPONENTS system log filesystem unit_test_framework test_exec_monitor REQUIRED )

ADD_EXECUTABLE( ${MY_APP_NAME} MACOSX_BUNDLE WIN32 ${TEST_JUDY_SRCS} ${COPY_RESOURCES} )
target_compile_features(${MY_APP_NAME} PRIVATE cxx_range_for)
TARGET_INCLUDE_DIRECTORIES( ${MY_APP_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} "../.." judy-static judymatch-static )
TARGET_LINK_LIBRARIES( ${MY_APP_NAME} "${Boost_LIBRARIES}" judy-static judymatch-static )



#--------------------------------------------------------------------------------
# Install the ${MY_APP_NAME} application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL( TARGETS ${MY_APP_NAME}
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)
