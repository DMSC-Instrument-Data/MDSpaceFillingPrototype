/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <omp.h>

#include <gflags/gflags.h>
#include <h5cpp/hdf5.hpp>

#include "DistributionGenerator.h"

using namespace hdf5;

using vector_type = std::vector<uint64_t>;

DEFINE_string(filename, "testdata.h5", "Output HDF5 file.");
DEFINE_string(datasetName, "data", "Name of generated dataset.");
DEFINE_uint64(chunks, 32, "Number of chunks to generate.");
DEFINE_uint64(chunkSize, 1000, "Number of data points in each chunk.");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  const size_t numElements = FLAGS_chunks * FLAGS_chunkSize;

  std::cout << "Generating random dataset \"" << FLAGS_datasetName
            << "\" in file \"" << FLAGS_filename << "\" with " << numElements
            << " elements.\n";

  /* Open HDF5 file */
  file::File f = file::create(FLAGS_filename, file::AccessFlags::TRUNCATE);
  node::Group root_group = f.root();

  /* Set chunking */
  property::DatasetCreationList dcpl;
  dcpl.layout(property::DatasetLayout::CHUNKED);
  dcpl.chunk({1000000});

  /* Create dataset */
  dataspace::Simple space{{numElements}, {dataspace::Simple::UNLIMITED}};
  auto type = datatype::create<vector_type>();
  node::Dataset dataset =
      root_group.create_dataset(FLAGS_datasetName, type, space, dcpl);

#pragma omp parallel for
  for (size_t i = 0; i < FLAGS_chunks; ++i) {
    /* Generate data */
    vector_type data;
    GenerateRandomPeakDataset_ClusteredPeaks(data, FLAGS_chunkSize);
    data.resize(FLAGS_chunkSize);

#pragma omp critical
    /* Write data */
    {
      dataspace::Hyperslab slab{
          {i * FLAGS_chunkSize}, {FLAGS_chunkSize}, {1}, {1},
      };
      dataset.write(data, slab);

      std::cout << "Chunk " << i << " done.\n";
    }
  }
}
