template <typename T>
void GenerateRandomUniformDataset(std::vector<T> &d, size_t n) {
  std::mt19937 generator(0);
  std::uniform_int_distribution<T> background(0, std::numeric_limits<T>::max());

  for (int i = 0; i < n; i++) {
    d.push_back(background(generator));
  }
}

using Peaks = std::vector<std::pair<float, float>>;

template <typename T>
void GenerateRandomUniformDatasetWithPeaks(std::vector<T> &d, size_t n,
                                           Peaks &peaks) {
  std::mt19937 generator(0);

  using PeakDist = std::cauchy_distribution<float>;
  std::vector<PeakDist> peakDistributions;
  for (const auto &p : peaks) {
    peakDistributions.emplace_back(p.first, p.second);
  }

  for (int i = 0; i < n / peakDistributions.size(); i++) {
    for (auto &p : peakDistributions) {
      float c = p(generator);

      /* Ensure value is in range (0.0, 1.0) */
      if (c > 1.0f || c < 0.0f) {
        i--;
        continue;
      }

      /* Scale to integer range (assumes unsigned to obtain full range) */
      d.push_back(std::numeric_limits<T>::max() * c);
    }
  }
}

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
