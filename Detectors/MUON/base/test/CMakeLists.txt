Set(MODULE_NAME "MUONBasetest")
Set(LIBRARY_MODULE_NAME "MUONBase")
Set(BUCKET_NAME muon_base_bucket)

O2_SETUP(NAME ${MODULE_NAME})

link_directories( ${LINK_DIRECTORIES})

Set(TEST_SRCS
        testChrono.cxx
        )

#target_include_directories(testChrono PUBLIC ${BOOST_INCLUDE_DIRS})

O2_GENERATE_TESTS(
        BUCKET_NAME ${BUCKET_NAME}
        MODULE_LIBRARY_NAME ${LIBRARY_MODULE_NAME}
        TEST_SRCS ${TEST_SRCS}
)