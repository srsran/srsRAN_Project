/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

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
  return static_cast<std::underlying_type<periodic_bsr_timer>::type>(timer);
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
};

} // namespace srsgnb
