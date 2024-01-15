/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/support/error_handling.h"

namespace srsran {

/// \c phr-PeriodicTimer, in PHR-Config, TS 38.331.
enum class phr_periodic_timer {
  sf10     = 10,
  sf20     = 20,
  sf50     = 50,
  sf100    = 100,
  sf200    = 200,
  sf500    = 500,
  sf1000   = 1000,
  infinity = 0
};

/// Return the value of \ref phr_periodic_timer.
inline std::underlying_type<phr_periodic_timer>::type phr_periodic_timer_to_value(phr_periodic_timer timer)
{
  return static_cast<std::underlying_type<phr_periodic_timer>::type>(timer);
}

/// \c phr-ProhibitTimer, in PHR-Config, TS 38.331.
enum class phr_prohibit_timer {
  sf0    = 0,
  sf10   = 10,
  sf20   = 20,
  sf50   = 50,
  sf100  = 100,
  sf200  = 200,
  sf500  = 500,
  sf1000 = 1000
};

/// Return the value of \ref phr_prohibit_timer.
inline std::underlying_type<phr_prohibit_timer>::type phr_prohibit_timer_to_value(phr_prohibit_timer timer)
{
  return static_cast<std::underlying_type<phr_prohibit_timer>::type>(timer);
}

/// Return the enum value of \ref phr-ProhibitTimer corresponding to the given unsigned value.
inline phr_prohibit_timer to_phr_prohibit_timer(unsigned prohib_timer)
{
  switch (prohib_timer) {
    case 0:
      return phr_prohibit_timer::sf0;
    case 10:
      return phr_prohibit_timer::sf10;
    case 20:
      return phr_prohibit_timer::sf20;
    case 50:
      return phr_prohibit_timer::sf50;
    case 100:
      return phr_prohibit_timer::sf100;
    case 200:
      return phr_prohibit_timer::sf200;
    case 500:
      return phr_prohibit_timer::sf500;
    case 1000:
      return phr_prohibit_timer::sf1000;
    default:
      report_fatal_error("Invalid phr-ProhibitTimer value={}", prohib_timer);
  }
}

/// \c phr-Tx-PowerFactorChange, in PHR-Config, TS 38.331.
enum class phr_tx_power_factor_change { db1 = 1, db3 = 3, db6 = 6, infinity = 0 };

/// Return the value of \ref phr_prohibit_timer.
inline std::underlying_type<phr_tx_power_factor_change>::type
phr_tx_pw_factor_change_to_value(phr_tx_power_factor_change timer)
{
  return static_cast<std::underlying_type<phr_tx_power_factor_change>::type>(timer);
}

/// \c phr-ModeOtherCG, in PHR-Config, TS 38.331.
enum class phr_mode_other_cg { real, virtual_ };

/// \c PHR-Config, TS 38.331.
struct phr_config {
  phr_periodic_timer         periodic_timer;
  phr_prohibit_timer         prohibit_timer;
  phr_tx_power_factor_change power_factor_change;
  bool                       multiple_phr;
  bool                       dummy;
  bool                       phr_type_to_other_cell;
  phr_mode_other_cg          phr_mode;

  bool operator==(const phr_config& rhs) const
  {
    return periodic_timer == rhs.periodic_timer && prohibit_timer == rhs.prohibit_timer &&
           power_factor_change == rhs.power_factor_change && multiple_phr == rhs.multiple_phr && dummy == rhs.dummy &&
           phr_type_to_other_cell == rhs.phr_type_to_other_cell && phr_mode == rhs.phr_mode;
  }
  bool operator!=(const phr_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
