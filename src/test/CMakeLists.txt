set(UNIT_TESTS
  BitInterleaving128BitTest
  BitInterleavingEigenTest
  BitInterleavingTest
  MDEventTest
  TestUtilTest
)

foreach(TEST ${UNIT_TESTS})
  UnitTest(
    NAME ${TEST}
    SOURCES ${TEST}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
  )
endforeach(TEST)
