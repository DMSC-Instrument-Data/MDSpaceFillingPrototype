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
#include <boost/multiprecision/cpp_int.hpp>

#pragma once

using uint128_t = boost::multiprecision::uint128_t;

template <size_t ND, typename IntT> using IntArray = Eigen::Array<IntT, ND, 1>;

template <size_t ND> using MDCoordinate = Eigen::Array<float, ND, 1>;

template <size_t ND> using MDSpaceBounds = Eigen::Array<float, ND, 2>;
template <size_t ND> using MDSpaceDimensions = Eigen::Array<float, ND, 1>;
template <size_t ND> using MDSpaceSteps = Eigen::Array<float, ND, 1>;

template <typename CoordT, size_t ND>
using AffineND = Eigen::Transform<CoordT, ND, Eigen::Affine>;

template <size_t ND> using BinIndices = Eigen::Matrix<size_t, 1, ND>;

struct Morton96 {
  uint64_t lower;
  uint32_t upper;

  Morton96(const uint128_t& cmpl);
  uint128_t to_uint128_t() const;

  friend bool inline operator<(const Morton96& a, const Morton96& b);
  friend bool inline operator>(const Morton96& a, const Morton96& b);
  friend bool inline operator==(const Morton96& a, const Morton96& b);
  friend bool inline operator!=(const Morton96& a, const Morton96& b);
};

Morton96::Morton96(const uint128_t &smpl) {
  union {
    uint64_t cmpl[2]; // complex
    uint128_t smpl; //simple
  } uni = {.smpl = smpl};
  upper = static_cast<uint32_t>(uni.cmpl[1]);
  lower = uni.cmpl[0];
}

uint128_t Morton96::to_uint128_t() const{
  union {
    uint64_t cmpl[2]; // complex
    uint128_t smpl; //simple
  } uni = {.cmpl = {lower, upper}};
  return uni.smpl;
}

bool operator<(const Morton96& a, const Morton96& b) {
  if(a.upper != b.upper)
    return a.upper < b.upper;
  else
    return a.lower < b.lower;
}

bool operator>(const Morton96& a, const Morton96& b) {
  if(a.upper != b.upper)
    return a.upper > b.upper;
  else
    return a.lower > b.lower;
}

bool operator==(const Morton96& a, const Morton96& b) {

  return a.lower == b.lower && a.upper == b.upper;
}

bool operator!=(const Morton96& a, const Morton96& b) {

  return a.lower != b.lower || a.upper != b.upper;
}

