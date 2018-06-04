Executable(
  NAME DatasetGenerator
  SOURCES DatasetGenerator.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} ${CMAKE_DL_LIBS}
)
