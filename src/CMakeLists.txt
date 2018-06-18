add_subdirectory(benchmark)
add_subdirectory(exe)
add_subdirectory(test)

Library(
  NAME
    MDSpaceFillingPrototype
  SOURCES
    EventNexusLoader.cpp
)
