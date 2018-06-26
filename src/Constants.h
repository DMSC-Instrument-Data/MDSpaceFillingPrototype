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
