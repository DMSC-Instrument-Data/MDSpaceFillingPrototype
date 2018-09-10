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

#include <Eigen/Dense>

#pragma once

template <size_t ND, typename IntT> using IntArray = Eigen::Array<IntT, ND, 1>;

template <size_t ND> using MDCoordinate = Eigen::Array<float, ND, 1>;

template <size_t ND> using MDSpaceBounds = Eigen::Array<float, ND, 2>;
template <size_t ND> using MDSpaceSteps = Eigen::Array<float, ND, 1>;

using MDSpaceBoundsX = Eigen::Array<float, Eigen::Dynamic, 2>;
using MDSpaceStepsX = Eigen::Array<float, Eigen::Dynamic, 1>;
