Executable(
  NAME DatasetGenerator
  SOURCES DatasetGenerator.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS}
)

Executable(
  NAME LoadInstrumentDemo
  SOURCES LoadInstrumentDemo.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)

Executable(
  NAME NexusParserDemo
  SOURCES NexusParserDemo.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)

Executable(
  NAME QConversionDemo
  SOURCES QConversionDemo.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)
