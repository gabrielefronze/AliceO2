set(MODULE_NAME "MIDFiltering")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
   src/Broadcaster.cxx
   src/MIDFilterAlgorithm.cxx
   src/MIDFilterDevice.cxx
   src/MIDMaskGeneratorAlgorithm.cxx
   src/MIDMaskGeneratorDevice.cxx
   src/MIDRatesComputerAlgorithm.cxx
   src/MIDRatesComputerDevice.cxx
#        src/FilteringChainDPL.cxx
#   src/runBroadcaster.cxx
#   src/runMIDFilterDevice.cxx
#   src/runMIDMaskGeneratorDevice.cxx
#   src/runMIDRatesComputerDevice.cxx
)

set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME mid_filtering_bucket)

O2_GENERATE_LIBRARY()

O2_GENERATE_EXECUTABLE(
  EXE_NAME runBroadcaster
  SOURCES src/runBroadcaster.cxx
  MODULE_LIBRARY_NAME ${LIBRARY_NAME}
  BUCKET_NAME ${BUCKET_NAME}
)

O2_GENERATE_EXECUTABLE(
  EXE_NAME runMIDFilterDevice
  SOURCES src/runMIDFilterDevice.cxx
  MODULE_LIBRARY_NAME ${LIBRARY_NAME}
  BUCKET_NAME ${BUCKET_NAME}
)

O2_GENERATE_EXECUTABLE(
  EXE_NAME runMIDMaskGeneratorDevice
  SOURCES src/runMIDMaskGeneratorDevice.cxx
  MODULE_LIBRARY_NAME ${LIBRARY_NAME}
  BUCKET_NAME ${BUCKET_NAME}
)

O2_GENERATE_EXECUTABLE(
  EXE_NAME runMIDRatesComputerDevice
  SOURCES src/runMIDRatesComputerDevice.cxx
  MODULE_LIBRARY_NAME ${LIBRARY_NAME}
  BUCKET_NAME ${BUCKET_NAME}
)

O2_FRAMEWORK_WORKFLOW(
        WORKFLOW_NAME "FilteringChainDPL"
        DETECTOR_BUCKETS mid_filtering_bucket
        SOURCES src/test_FilteringChainDPL.cxx src/FilteringChainDPL.cxx
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/runBroadcaster.json DESTINATION etc/config)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/runMIDFilterDevice.json DESTINATION etc/config)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/runMIDMaskGeneratorDevice.json DESTINATION etc/config)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/runMIDRatesComputerDevice.json DESTINATION etc/config)
