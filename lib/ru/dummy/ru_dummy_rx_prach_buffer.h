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

#include "srsran/adt/tensor.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/support/complex_normal_random.h"

namespace srsran {

/// Implements a PRACH buffer reader that contains always the same random data.
class ru_dummy_rx_prach_buffer : private prach_buffer
{
public:
  ru_dummy_rx_prach_buffer(unsigned seed, unsigned max_nof_ports)
  {
    // Resize PRACH buffer data tp the maximum buffer size.
    data.resize({prach_constants::SHORT_SEQUENCE_LENGTH,
                 prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS,
                 prach_constants::MAX_NOF_PRACH_FD_OCCASIONS,
                 prach_constants::MAX_NOF_PRACH_TD_OCCASIONS,
                 max_nof_ports});

    // Fill PRACH buffer with random data.
    complex_normal_distribution<cf_t> dist(0, 1);
    std::mt19937                      rgen(seed);
    span<cf_t>                        data_view = data.get_data();
    std::generate(data_view.begin(), data_view.end(), [&dist, &rgen]() { return dist(rgen); });
  }

  /// Gets the the PRACH buffer.
  prach_buffer& get_buffer(const prach_buffer_context& context)
  {
    unsigned nof_symbols, sequence_length;

    // Derive parameters from the PRACH format.
    if (is_long_preamble(context.format)) {
      prach_preamble_information info = get_prach_preamble_long_info(context.format);
      nof_symbols                     = static_cast<unsigned>(info.symbol_length.to_seconds() * ra_scs_to_Hz(info.scs));
      sequence_length                 = info.sequence_length;
    } else {
      prach_preamble_information info =
          get_prach_preamble_short_info(context.format, to_ra_subcarrier_spacing(context.pusch_scs), false);
      nof_symbols     = static_cast<unsigned>(info.symbol_length.to_seconds() * ra_scs_to_Hz(info.scs));
      sequence_length = info.sequence_length;
    }

    // Derive other parameters.
    unsigned nof_ports        = context.ports.size();
    unsigned nof_td_occasions = context.nof_td_occasions;
    unsigned nof_fd_occasions = context.nof_fd_occasions;

    // Resize data to match context specifications.
    if ((sequence_length != data.get_dimension_size(dims::re)) ||
        (nof_symbols != data.get_dimension_size(dims::symbol)) ||
        (nof_fd_occasions != data.get_dimension_size(dims::fd_occasion)) ||
        (nof_td_occasions != data.get_dimension_size(dims::td_occasion)) ||
        (nof_ports != data.get_dimension_size(dims::port))) {
      data.resize({sequence_length, nof_symbols, nof_fd_occasions, nof_td_occasions, nof_ports});
    }

    return *this;
  }

private:
  // See interface for documentation.
  unsigned int get_max_nof_ports() const override { return data.get_dimension_size(dims::port); }

  // See interface for documentation.
  unsigned get_max_nof_td_occasions() const override { return data.get_dimension_size(dims::td_occasion); }

  // See interface for documentation.
  unsigned get_max_nof_fd_occasions() const override { return data.get_dimension_size(dims::fd_occasion); }

  // See interface for documentation.
  unsigned get_max_nof_symbols() const override { return data.get_dimension_size(dims::symbol); }

  // See interface for documentation.
  unsigned get_sequence_length() const override { return data.get_dimension_size(dims::re); }

  // See interface for documentation.
  span<cf_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) override
  {
    srsran_assert(i_port < get_max_nof_ports(),
                  "The port index (i.e., {}) exceeds the maximum number of ports (i.e., {}).",
                  i_port,
                  get_max_nof_ports());
    srsran_assert(i_td_occasion < get_max_nof_td_occasions(),
                  "The time-domain occasion (i.e., {}) exceeds the maximum number of time-domain occasions (i.e., {}).",
                  i_td_occasion,
                  get_max_nof_td_occasions());
    srsran_assert(
        i_fd_occasion < get_max_nof_fd_occasions(),
        "The frequency-domain occasion (i.e., {}) exceeds the maximum number of frequency-domain occasions (i.e., {}).",
        i_fd_occasion,
        get_max_nof_fd_occasions());
    srsran_assert(i_symbol < get_max_nof_symbols(),
                  "The symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {}).",
                  i_symbol,
                  get_max_nof_symbols());
    return data.get_view({i_symbol, i_fd_occasion, i_td_occasion, i_port});
  }

  // See interface for documentation.
  span<const cf_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override
  {
    srsran_assert(i_port < get_max_nof_ports(),
                  "The port index (i.e., {}) exceeds the maximum number of ports (i.e., {}).",
                  i_port,
                  get_max_nof_ports());
    srsran_assert(i_td_occasion < get_max_nof_td_occasions(),
                  "The time-domain occasion (i.e., {}) exceeds the maximum number of time-domain occasions (i.e., {}).",
                  i_td_occasion,
                  get_max_nof_td_occasions());
    srsran_assert(
        i_fd_occasion < get_max_nof_fd_occasions(),
        "The frequency-domain occasion (i.e., {}) exceeds the maximum number of frequency-domain occasions (i.e., {}).",
        i_fd_occasion,
        get_max_nof_fd_occasions());
    srsran_assert(i_symbol < get_max_nof_symbols(),
                  "The symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {}).",
                  i_symbol,
                  get_max_nof_symbols());
    return data.get_view({i_symbol, i_fd_occasion, i_td_occasion, i_port});
  }

  /// Data storage dimensions.
  enum class dims : unsigned {
    re          = 0,
    symbol      = 1,
    fd_occasion = 2,
    td_occasion = 3,
    port        = 4,
    count       = 5,
  };

  /// Data storage.
  dynamic_tensor<static_cast<std::underlying_type_t<dims>>(dims::count), cf_t, dims> data;
};

} // namespace srsran
