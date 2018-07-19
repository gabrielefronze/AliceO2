O2_SETUP(NAME MIDFilteringTest)
set(BUCKET_NAME mid_filtering_test_bucket)

O2_GENERATE_TESTS(
        BUCKET_NAME ${BUCKET_NAME}
        TEST_SRCS testHitCounter.cxx
)

O2_GENERATE_TESTS(
        BUCKET_NAME ${BUCKET_NAME}
        TEST_SRCS testMaskGenerator.cxx
)

O2_GENERATE_TESTS(
        BUCKET_NAME ${BUCKET_NAME}
        TEST_SRCS testFilter.cxx
)

#
#if (benchmark_FOUND)
#    O2_GENERATE_EXECUTABLE(
#            EXE_NAME benchClusterizer
#            SOURCES bench_Clusterizer.cxx
#            BUCKET_NAME ${BUCKET_NAME})
#endif ()