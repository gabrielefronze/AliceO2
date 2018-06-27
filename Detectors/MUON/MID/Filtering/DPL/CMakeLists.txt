O2_SETUP(NAME MIDFilteringDPL)
set(BUCKET_NAME mid_filtering_DPL_bucket)

set(SRCS
        MIDFilteringDPL.cxx
        )

#add_executable(MIDFilteringDPL MIDFilteringDPL.cxx)
#target_link_libraries(MIDFiltering Framework MIDBase)
#install(TARGETS MIDFilteringDPL RUNTIME DESTINATION bin)

#O2_GENERATE_EXECUTABLE(
#        EXE_NAME "MIDFilteringDPL"
#        SOURCES "MIDFilteringDPL.cxx"
#        MODULE_LIBRARY_NAME ${LIBRARY_NAME}
#        BUCKET_NAME ${BUCKET_NAME}
#)