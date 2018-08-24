#include <vector>

#include <h5cpp/hdf5.hpp>

#pragma once

template <typename T>
void resize_and_read_dataset_range(std::vector<T> &data,
                                   hdf5::node::Dataset dataset, size_t start,
                                   size_t end) {
  hdf5::dataspace::Hyperslab slab({start}, {end - start}, {1}, {1});
  data.resize(slab.block()[0]);
  dataset.read(data, slab);
}

template <typename T>
void resize_and_read_dataset(std::vector<T> &data,
                             hdf5::node::Dataset dataset) {
  data.resize(dataset.dataspace().size());
  dataset.read(data);
}
