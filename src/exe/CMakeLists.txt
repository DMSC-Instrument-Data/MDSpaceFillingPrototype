Executable(
  NAME DatasetGenerator
  SOURCES DatasetGenerator.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS}
)

Executable(
  NAME QConversionDemo
  SOURCES QConversionDemo.cpp scoped_wallclock_timer.cpp ArgumentsParsing.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)

Executable(
  NAME MergeDemo
  SOURCES MergeDemo.cpp scoped_wallclock_timer.cpp ArgumentsParsing.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)
