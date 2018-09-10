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
