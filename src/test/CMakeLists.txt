set(UNIT_TESTS
  BitInterleaving128BitTest
  BitInterleavingEigenTest
  BitInterleavingTest
  CoordinateConversionTest
  EventDistribution2DTest
  EventToMDEventConversionTest
  InstrumentTest
  IsisEventNexusLoaderTest
  MantidEventNexusLoaderTest
  MDBox2DTest
  MDBox4DTest
  MDEventTest
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
