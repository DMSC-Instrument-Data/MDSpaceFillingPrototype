set(UNIT_TESTS
  BitInterleaving128BitTest
  BitInterleavingEigenTest
  BitInterleavingTest
  CoordinateConversionTest
  MDEventTest
  EventNexusLoaderTest
  TestUtilTest
)

foreach(TEST ${UNIT_TESTS})
  UnitTest(
    NAME ${TEST}
    SOURCES ${TEST}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
    LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
  )
endforeach(TEST)

file(COPY test_data DESTINATION .)
