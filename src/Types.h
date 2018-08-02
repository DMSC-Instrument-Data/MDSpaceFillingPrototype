#include <Eigen/Dense>

#pragma once

template <size_t ND, typename IntT> using IntArray = Eigen::Array<IntT, ND, 1>;

template <size_t ND> using MDCoordinate = Eigen::Array<float, ND, 1>;
template <size_t ND> using MDSpaceBounds = Eigen::Array<float, ND, 2>;
template <size_t ND> using MDSpaceSteps = Eigen::Array<float, ND, 1>;
