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

#include "Constants.h"

#include <cinttypes>

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision::literals;

template <>
const uint32_t MortonMask<2, uint16_t, uint32_t>::mask[] = {
    0x55555555,
    0xaaaaaaaa,
};

template <>
const uint64_t MortonMask<3, uint16_t, uint64_t>::mask[] = {
    0x249249249249,
    0x492492492492,
    0x924924924924,
};

template <>
const uint32_t MortonMask<4, uint8_t, uint32_t>::mask[] = {
    0x11111111,
    0x22222222,
    0x44444444,
    0x88888888,
};

template <>
const uint64_t MortonMask<4, uint16_t, uint64_t>::mask[] = {
    0x1111111111111111,
    0x2222222222222222,
    0x4444444444444444,
    0x8888888888888888,
};

template <>
const boost::multiprecision::uint128_t
    MortonMask<4, uint32_t, boost::multiprecision::uint128_t>::mask[] = {
        0x11111111111111111111111111111111_cppui128,
        0x22222222222222222222222222222222_cppui128,
        0x44444444444444444444444444444444_cppui128,
        0x88888888888888888888888888888888_cppui128,
};
