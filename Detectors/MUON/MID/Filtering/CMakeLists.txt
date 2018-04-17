set(MODULE_NAME "MIDFiltering")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
        src/HitCounter.cxx
        src/MaskGenerator.cxx
        src/Filter.cxx
)

set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME mid_filtering_bucket)

O2_GENERATE_LIBRARY()
