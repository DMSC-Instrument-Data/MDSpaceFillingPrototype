#include <vector>
#include <random>

#pragma once

/**
 * Generates a random dataset with uniform distribution.
 *
 * @param d Reference to container
 * @param n Number of elements to generate
 */
template <typename T>
void GenerateRandomUniformDataset(std::vector<T> &d, size_t n) {
  std::mt19937 generator(0);
  std::uniform_int_distribution<T> background(0, std::numeric_limits<T>::max());

  for (int i = 0; i < n; i++) {
    d.push_back(background(generator));
  }
}

using Peaks = std::vector<std::pair<float, float>>;

/**
 * Generates a random dataset with several peaks in the distribution.
 *
 * Uses the std::cauchy_distribution
 * (http://en.cppreference.com/w/cpp/numeric/random/cauchy_distribution).
 *
 * Note that after execution d.size() == n is not guaranteed.
 *
 * @param d Reference to container
 * @param n Number of elements to generate
 * @param peaks List of (a, b) pairs used for distribution
 */
template <typename T>
void GenerateRandomPeakDataset(std::vector<T> &d, size_t n, Peaks &peaks) {
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

/**
 * Generates a random dataset with several predefined peaks in the distribution.
 *
 * Note that after execution d.size() == n is not guaranteed.
 *
 * @param d Reference to container
 * @param n Number of elements to generate
 *
 * @param see GenerateRandomPeakDataset
 */
template <typename T>
void GenerateRandomPeakDataset_ClusteredPeaks(std::vector<T> &d, size_t n) {
  std::vector<std::pair<float, float>> peaks = {
      // clang-format off
      {0.1f,    0.001f},
      {0.11f,   0.001f},
      {0.12f,   0.001f},
      {0.33f,   0.001f},
      {0.35f,   0.001f},
      {0.37f,   0.001f},
      {0.42f,   0.001f},
      {0.43f,   0.001f},
      {0.44f,   0.001f},
      {0.51f,   0.001f},
      {0.5f,    0.001f},
      {0.505f,  0.001f},
      {0.51f,   0.001f},
      {0.57f,   0.001f},
      {0.61f,   0.001f},
      {0.62f,   0.001f},
      {0.625f,  0.001f},
      {0.75f,   0.001f},
      {0.755f,  0.001f},
      {0.76f,   0.001f},
      {0.765f,  0.001f},
      {0.77f,   0.001f},
      {0.8f,    0.001f},
      {0.83f,   0.001f},
      {0.84f,   0.001f},
      {0.9f,    0.001f},
      {0.91f,   0.001f},
      // clang-format on
  };

  GenerateRandomPeakDataset(d, n, peaks);
}

#define GENERATE_DATASET(name, type, generator, n)                             \
  std::cout << "Generating dataset " #name "\n";                               \
  std::vector<type> name;                                                      \
  generator(name, n)
