set(MODULE_NAME "MUONBase")

O2_SETUP(NAME ${MODULE_NAME})

link_directories( ${LINK_DIRECTORIES})

set(SRCS
        src/Mapping.cxx
        src/Serializer.cxx
        src/Deserializer.cxx
        src/OccupancyMapping.cxx
        )

set(HEADERS
        include/${MODULE_NAME}/Mapping.h
        include/${MODULE_NAME}/Deserializer.h
        include/${MODULE_NAME}/Serializer.h
        include/${MODULE_NAME}/Enums.h
        include/${MODULE_NAME}/OccupancyMapping.h
        include/${MODULE_NAME}/Chrono.h
        )

Set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME muon_base_bucket)

O2_GENERATE_LIBRARY()

# Repeating because O2_GENERATE_LIBRARY hides previous config
Set(MODULE_NAME "MUONBase")
Set(LIBRARY_MODULE_NAME "MUONBase")
Set(BUCKET_NAME muon_base_bucket)

link_directories( ${LINK_DIRECTORIES})

Set(TEST_SRCS
        test/testChrono.cxx
        )

O2_GENERATE_TESTS(
        BUCKET_NAME ${BUCKET_NAME}
        MODULE_LIBRARY_NAME ${LIBRARY_MODULE_NAME}
        TEST_SRCS ${TEST_SRCS}
)
