Executable(
  NAME DatasetGenerator
  SOURCES DatasetGenerator.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS}
)

Executable(
  NAME NexusParserDemo
  SOURCES NexusParserDemo
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)
