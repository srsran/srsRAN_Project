/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/prach_buffer.h"

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
