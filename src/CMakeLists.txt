add_subdirectory(benchmark)
add_subdirectory(exe)
add_subdirectory(test)

Library(
  NAME
    MDSpaceFillingPrototype
  SOURCES
    EventToMDEventConversion.cpp
    Instrument.cpp
    IsisEventNexusLoader.cpp
  LIBRARIES
    ${CONAN_LIBS_H5CPP}
)
