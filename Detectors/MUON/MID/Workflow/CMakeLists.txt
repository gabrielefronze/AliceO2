set(MODULE_NAME "MIDWorkflow")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
        src/ClusterizerSpec.cxx
        src/TrackerSpec.cxx
        src/MIDWorkflow.cxx
        src/ColdataStreamerSpec.cxx
        src/SinkSpec.cxx
        src/DecoderSpec.cxx
        src/HitCounterSpec.cxx
        src/MaskGeneratorSpec.cxx
        )

set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME mid_workflow_bucket)

O2_GENERATE_LIBRARY()

add_executable(mid-workflow ${SRCS})
target_link_libraries(mid-workflow Framework MIDWorkflow)
install(TARGETS mid-workflow RUNTIME DESTINATION bin)