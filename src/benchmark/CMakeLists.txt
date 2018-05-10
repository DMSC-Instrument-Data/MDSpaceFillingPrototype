add_executable(
  DummyBenchmark
  DummyBenchmark.cpp
)
target_link_libraries(
  DummyBenchmark
  gbenchmark
  pthread
  ${CONAN_LIBS}
)
add_test(DummyBenchmark DummyBenchmark)
