set(MODULE_NAME "MIDFiltering")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
        src/Decoder.cxx
        src/HitCounter.cxx
        src/MaskGenerator.cxx
        src/Filter.cxx
)

set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME mid_filtering_bucket)

O2_GENERATE_LIBRARY()

add_subdirectory(test)
add_subdirectory(DPL)
