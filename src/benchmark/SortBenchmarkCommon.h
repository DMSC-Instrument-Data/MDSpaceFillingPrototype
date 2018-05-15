template <typename T>
void GenerateRandomUniformDataset(std::vector<T> &d, size_t n) {
  std::mt19937 generator(0);
  std::uniform_int_distribution<T> background(0, std::numeric_limits<T>::max());

  for (int i = 0; i < n; i++) {
    d.push_back(background(generator));
  }
}

template <typename T>
void GenerateRandomUniformDatasetWithSinglePeak(std::vector<T> &d, size_t n) {
  std::mt19937 generator(0);
  std::cauchy_distribution<float> peak(0.4f, 0.05f);

  for (int i = 0; i < n; i++) {
    float c = peak(generator);

    /* Ensure value is in range (0.0, 1.0) */
    if (c > 1.0f || c < 0.0f) {
      i--;
      continue;
    }

    /* Scale to integer range (assumes unsigned to obtain full range) */
    d.push_back(std::numeric_limits<T>::max() * c);
  }
}

#define GENERATE_DATASET(name, type, generator, n)                             \
  std::cout << "Generating dataset " #name "\n";                               \
  std::vector<type> name;                                                      \
  generator(name, n)

#define REGISTER_SORT_BENCHMARK(type, container, data, sort)                   \
  benchmark::RegisterBenchmark(#type " " #container " " #sort " " #data,       \
                               [=](benchmark::State &state) {                  \
                                 for (auto _ : state) {                        \
                                   state.PauseTiming();                        \
                                   container<type> v;                          \
                                   std::copy(data.begin(), data.end(),         \
                                             std::back_inserter(v));           \
                                   state.ResumeTiming();                       \
                                   sort(v.begin(), v.end());                   \
                                 }                                             \
                               })                                              \
      ->Unit(benchmark::kMillisecond);
