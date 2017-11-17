set(MODULE_NAME "MUONBase")

O2_SETUP(NAME ${MODULE_NAME})

link_directories( ${LINK_DIRECTORIES})

set(SRCS
        src/Mapping.cxx
        src/Deserializer.cxx
        src/OccupancyMapping.cxx
        )

set(HEADERS
        include/${MODULE_NAME}/Mapping.h
        include/${MODULE_NAME}/Deserializer.h
        include/${MODULE_NAME}/Enums.h
        include/${MODULE_NAME}/OccupancyMapping.h
        include/${MODULE_NAME}/Chrono.h
        )
Set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME muon_base_bucket)

O2_GENERATE_LIBRARY()

add_subdirectory(test)
