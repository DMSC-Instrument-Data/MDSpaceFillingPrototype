#include <benchmark/benchmark.h>

#include <h5cpp/hdf5.hpp>

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

using namespace hdf5;

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  const std::string filename("testdata.h5");
  const std::string datasetName("data");

  /* Open HDF5 file */
  file::File f = file::open(filename, file::AccessFlags::READONLY);
  node::Group root_group = f.root();

  /* Open dataset and get dimensions */
  node::Dataset dataset = root_group[datasetName];
  dataspace::Dataspace dataspace = dataset.dataspace();

  /* Create container to be sorted */
  std::cout << "Allocating vector of size " << dataspace.size() << '\n';
  std::vector<uint64_t> data(dataspace.size());

#define BM_SORT(sort)                                                          \
  benchmark::RegisterBenchmark(#sort, [&](benchmark::State & state) {          \
    for (auto _ : state) {                                                     \
      state.PauseTiming();                                                     \
      dataset.read(data);                                                      \
      state.ResumeTiming();                                                    \
      sort(data.begin(), data.end());                                          \
    }                                                                          \
  })->Unit(benchmark::kMillisecond)->UseRealTime();

  BM_SORT(boost::sort::block_indirect_sort);
  BM_SORT(boost::sort::sample_sort);
  BM_SORT(tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
