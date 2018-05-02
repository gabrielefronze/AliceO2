set(MODULE_NAME "MIDSimulation")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
  src/ChamberHV.cxx
  src/ChamberResponse.cxx
  src/ChamberResponseParams.cxx
  src/Digitizer.cxx
)

set(NO_DICT_HEADERS
  include/${MODULE_NAME}/ChamberHV.h
  include/${MODULE_NAME}/ChamberResponse.h
  include/${MODULE_NAME}/ChamberResponseParams.h
  include/${MODULE_NAME}/Digitizer.h
)

set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME mid_simulation_bucket)

O2_GENERATE_LIBRARY()
