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

#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {
namespace ofh {

/// Returns the maximum value between the minimum T1a values in symbol units.
inline unsigned
get_biggest_min_tx_parameter_in_symbols(const du_tx_window_timing_parameters&           tx_timing_params,
                                        const std::chrono::duration<double, std::nano>& symbol_duration_ns)
{
  auto max_value =
      std::max({tx_timing_params.T1a_min_cp_dl, tx_timing_params.T1a_min_cp_ul, tx_timing_params.T1a_min_up});

  return std::floor(max_value / symbol_duration_ns);
}

/// Returns duration of the OFH downlink processing plus the transmission window in symbol units.
inline unsigned calculate_nof_symbols_before_ota(cyclic_prefix                        cp,
                                                 subcarrier_spacing                   scs,
                                                 std::chrono::microseconds            dl_processing_time,
                                                 const du_tx_window_timing_parameters tx_timing_params)
{
  unsigned nof_symbols = get_nsymb_per_slot(cp);
  auto     symbol_duration_ns =
      std::chrono::duration<double, std::nano>(1e6 / (nof_symbols * get_nof_slots_per_subframe(scs)));
  unsigned dl_processing_time_in_symbols = std::floor(dl_processing_time / symbol_duration_ns);

  return dl_processing_time_in_symbols + get_biggest_min_tx_parameter_in_symbols(tx_timing_params, symbol_duration_ns);
}

} // namespace ofh
} // namespace srsran
