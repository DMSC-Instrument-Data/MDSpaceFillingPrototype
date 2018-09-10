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

#include <cmath>

#pragma once

/** Planck constant in J*s. Taken from <http://physics.nist.gov/cuu/Constants>
    on 2007-10-31 and confirmed again on 2010-12-28. */
static constexpr double h = 6.62606896e-34;

/** Planck constant in J*s, divided by 2 PI. */
static constexpr double h_bar = h / (2 * M_PI);

/** Mass of the neutron in kg. Taken from
 * <http://physics.nist.gov/cuu/Constants> on 30/10/2007. */
static constexpr double NeutronMass = 1.674927211e-27;
