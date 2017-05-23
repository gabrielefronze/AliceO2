set(MODULE_NAME "MUONMonitoring")

O2_SETUP(NAME ${MODULE_NAME})

link_directories( ${LINK_DIRECTORIES})

set(SRCS
#   src/Mapping.cxx
#   src/Deserializer.cxx
)

set(HEADERS
#   include/${MODULE_NAME}/Mapping.h
#   include/${MODULE_NAME}/Deserializer.h
)
Set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME muon_monitoring_bucket)

O2_GENERATE_LIBRARY()
