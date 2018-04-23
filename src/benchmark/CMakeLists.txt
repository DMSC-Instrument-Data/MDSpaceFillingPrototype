add_executable(
  DummyBenchmark
  DummyBenchmark.cpp
)
target_link_libraries(
  DummyBenchmark
  gbenchmark
  pthread
)
add_test(DummyBenchmark DummyBenchmark)
