find_package(Boost COMPONENTS unit_test_framework)


add_executable(testChrono
        testChrono.cxx)

target_include_directories(testChrono PUBLIC ${CMAKE_SOURCE_DIR} ${BOOST_INCLUDE_DIRS})
target_link_libraries(testChrono PUBLIC Boost::unit_test_framework)
add_test(NAME testChrono COMMAND testChrono)
