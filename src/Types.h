#include <Eigen/Dense>

#pragma once

template <size_t ND> using MDCoordinate = Eigen::Array<float, ND, 1>;
template <size_t ND> using MDSpace = Eigen::Array<float, ND, 2>;
