add_subdirectory(benchmark)
add_subdirectory(exe)
add_subdirectory(test)

Library(
  NAME
    MDSpaceFillingPrototype
  SOURCES
    Constants.cpp
    EventToMDEventConversion.cpp
    Instrument.cpp
    IsisEventNexusLoader.cpp
    MantidEventNexusLoader.cpp
  LIBRARIES
    ${CONAN_LIBS_H5CPP}
)
