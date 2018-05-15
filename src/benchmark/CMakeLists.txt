add_executable(
  DummyBenchmark
  DummyBenchmark.cpp
)
target_link_libraries(
  DummyBenchmark
  ${GBENCH_LIBRARIES}
  ${CONAN_LIBS}
)
add_test(DummyBenchmark DummyBenchmark)
add_dependencies(DummyBenchmark benchmark)
