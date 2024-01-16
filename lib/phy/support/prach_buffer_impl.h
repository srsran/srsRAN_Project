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

#include "srsran/adt/static_vector.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/prach_buffer.h"

using namespace srsran;

namespace srsran {

/// PRACH buffer with a fix storage size at initialization.
class prach_buffer_impl : public prach_buffer
{
private:
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

public:
  /// Creates a PRACH buffer from the maximum parameters depending on the configuration.
  prach_buffer_impl(unsigned max_nof_ports,
                    unsigned max_nof_td_occasions,
                    unsigned max_nof_fd_occasions,
                    unsigned max_nof_symbols,
                    unsigned sequence_length)
  {
    data.resize({sequence_length, max_nof_symbols, max_nof_fd_occasions, max_nof_td_occasions, max_nof_ports});
  }

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
};

} // namespace srsran
