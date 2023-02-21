/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/prach/prach_preamble_information.h"

using namespace srsran;

prach_preamble_information srsran::get_prach_preamble_long_info(preamble_format format)
{
  srsran_assert(format.is_long_preamble(), "Invalid preamble format. It must be a long preamble.");
  switch (format) {
    case preamble_format::FORMAT0:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(24576),
              phy_time_unit::from_units_of_kappa(3168),
              true};
    case preamble_format::FORMAT1:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(2 * 24576),
              phy_time_unit::from_units_of_kappa(21024),
              true};
    case preamble_format::FORMAT2:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(4 * 24576),
              phy_time_unit::from_units_of_kappa(4688),
              true};
    case preamble_format::FORMAT3:
    default:
      return {839U,
              prach_subcarrier_spacing::kHz5,
              phy_time_unit::from_units_of_kappa(4 * 6144),
              phy_time_unit::from_units_of_kappa(3168),
              true};
  }
}