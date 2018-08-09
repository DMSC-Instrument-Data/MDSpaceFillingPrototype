Executable(
  NAME DatasetGenerator
  SOURCES DatasetGenerator.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS}
)

Executable(
  NAME QConversionDemo
  SOURCES QConversionDemo.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)
