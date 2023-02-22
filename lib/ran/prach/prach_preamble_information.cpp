/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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