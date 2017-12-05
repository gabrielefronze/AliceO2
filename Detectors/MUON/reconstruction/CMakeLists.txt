set(MODULE_NAME "MUONReconstruction")

O2_SETUP(NAME ${MODULE_NAME})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/runMIDFilter.json ${CMAKE_BINARY_DIR}/bin/config/runMIDFilter.json)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/runMIDMaskGenerator.json ${CMAKE_BINARY_DIR}/bin/config/runMIDMaskGenerator.json)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/runMIDRatesComputer.json ${CMAKE_BINARY_DIR}/bin/config/runMIDRatesComputer.json)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/runBroadcaster.json ${CMAKE_BINARY_DIR}/bin/config/runBroadcaster.json)



link_directories(${LINK_DIRECTORIES})

set(SRCS
        src/MIDFilterDevice.cxx
        src/MIDMaskGeneratorDevice.cxx
        src/MIDRatesComputerDevice.cxx
        src/Broadcaster.cxx
        )

set(HEADERS
        include/${MODULE_NAME}/MIDFilterDevice.h
        include/${MODULE_NAME}/MIDMaskGeneratorDevice.h
        include/${MODULE_NAME}/MIDRatesComputerDevice.h
        include/${MODULE_NAME}/Broadcaster.h
        )

Set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME muon_reconstruction_bucket)

O2_GENERATE_LIBRARY()

Set(Exe_Names
        runMIDFilterDevice
        runMIDMaskGeneratorDevice
        runMIDRatesComputerDevice
        runBroadcaster
        )

set(Exe_Source
        src/runMIDFilterDevice.cxx
        src/runMIDMaskGeneratorDevice.cxx
        src/runMIDRatesComputerDevice.cxx
        src/runBroadcaster.cxx
        )

# todo we repeat ourselves because the above macro dares deleting the variables we pass to it.
set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME muon_reconstruction_bucket)

list(LENGTH Exe_Names _length)
math(EXPR _length ${_length}-1)

ForEach (_file RANGE 0 ${_length}) # loop over a range because we traverse 2 lists and not 1
    list(GET Exe_Names ${_file} _name)
    list(GET Exe_Source ${_file} _src)
    O2_GENERATE_EXECUTABLE(
            EXE_NAME ${_name}
            SOURCES ${_src}
            MODULE_LIBRARY_NAME ${LIBRARY_NAME}
            BUCKET_NAME ${BUCKET_NAME}
    )
EndForEach (_file RANGE 0 ${_length})

install(FILES ${CMAKE_BINARY_DIR}/bin/config/runMIDFilter.json
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/config)
install(FILES ${CMAKE_BINARY_DIR}/bin/config/runMIDMaskGenerator.json
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/config)
install(FILES ${CMAKE_BINARY_DIR}/bin/config/runMIDRatesComputer.json
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/config)
install(FILES ${CMAKE_BINARY_DIR}/bin/config/runBroadcaster.json
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/config)
