add_executable(
  MDEventTest
  MDEventTest.cpp
)
target_include_directories(
  MDEventTest PUBLIC
  ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(
  MDEventTest
  ${CONAN_LIBS_GTEST}
)
add_test(MDEventTest MDEventTest)
