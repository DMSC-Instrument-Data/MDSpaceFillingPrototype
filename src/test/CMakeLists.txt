set(UNIT_TESTS
  BitInterleaving128BitTest
  BitInterleaving256BitTest
  BitInterleavingEigenTest
  BitInterleavingTest
  CoordinateConversionTest
  EventStorageTest
  EventToMDEventConversionTest
  InstrumentTest
  IsisEventNexusLoaderTest
  MantidEventNexusLoaderTest
  MDBox2DTest
  MDBox3DTest
  MDBox4DTest
  MDEventTest
  MergeTest
  Morton96Test
  RebinTest
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
