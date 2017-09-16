SET( MY_APP_NAME "TestJudymatch" )

# external modules
FILE( GLOB_RECURSE TEST_JUDYMATCH_SRCS "*.cpp" "*.hpp" "*.tcc" )
FIND_PACKAGE( Boost COMPONENTS system log filesystem unit_test_framework test_exec_monitor REQUIRED )

ADD_EXECUTABLE( ${MY_APP_NAME} MACOSX_BUNDLE WIN32 ${TEST_JUDYMATCH_SRCS} )
target_compile_features(${MY_APP_NAME} PRIVATE cxx_range_for)
TARGET_INCLUDE_DIRECTORIES( ${MY_APP_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} "../.." judy-static judymatch-static )
TARGET_LINK_LIBRARIES( ${MY_APP_NAME} "${Boost_LIBRARIES}" judy-static judymatch-static )

add_test(${MY_APP_NAME} ${MY_APP_NAME})
