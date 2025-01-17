/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_validator.h"

namespace srsran {

/// Implements a PRS generator configuration validator.
class prs_generator_validator_impl : public prs_generator_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const prs_generator_configuration& config) const override
  {
    static constexpr interval<uint16_t, true> n_id_prs_range(0, 4095);
    if (!n_id_prs_range.contains(config.n_id_prs)) {
      return make_unexpected(
          fmt::format("The sequence identifier (i.e., {}) is out of the range {}.", config.n_id_prs, n_id_prs_range));
    }

    if (!prs_valid_num_symbols_and_comb_size(config.duration, config.comb_size)) {
      return make_unexpected(fmt::format("Invalid combination of duration (i.e., {}) and comb size (i.e., {}).",
                                         fmt::underlying(config.duration),
                                         fmt::underlying(config.comb_size)));
    }

    interval<uint8_t> comb_offset_range(0, static_cast<uint8_t>(config.comb_size));
    if (!comb_offset_range.contains(config.comb_offset)) {
      return make_unexpected(
          fmt::format("The comb offset (i.e., {}) is out of the range {}.", config.comb_offset, comb_offset_range));
    }

    unsigned nof_symbols_slot = get_nsymb_per_slot(config.cp);
    if (config.start_symbol + fmt::underlying(config.duration) > nof_symbols_slot) {
      return make_unexpected(
          fmt::format("Start symbol (i.e., {}) plus duration (i.e., {}) exceed the slot duration (i.e., {}).",
                      config.start_symbol,
                      fmt::underlying(config.duration),
                      nof_symbols_slot));
    }

    if (config.precoding.get_nof_layers() != 1) {
      return make_unexpected(
          fmt::format("The number of layers (i.e., {}) must be one.", config.precoding.get_nof_layers()));
    }

    if (config.precoding.get_nof_prg() != 1) {
      return make_unexpected(fmt::format("The number of PRG (i.e., {}) must be one.", config.precoding.get_nof_prg()));
    }

    static constexpr interval<unsigned, true> bandwidth_range(24, 276);
    if (!bandwidth_range.contains(config.freq_alloc.length())) {
      return make_unexpected(fmt::format(
          "The bandwidth (i.e., {} PRB) is out of the range {}.", config.freq_alloc.length(), bandwidth_range));
    }

    if (config.freq_alloc.length() % 4 != 0) {
      return make_unexpected(
          fmt::format("The bandwidth (i.e., {} PRB) must be multiple of 4.", config.freq_alloc.length()));
    }

    static constexpr interval<unsigned, true> prb_start_range(0, 2176);
    if (!prb_start_range.contains(config.prb_start)) {
      return make_unexpected(
          fmt::format("The PRB start (i.e., {}) is out of the range {}.", config.prb_start, prb_start_range));
    }

    return default_success_t();
  }
};

} // namespace srsran
