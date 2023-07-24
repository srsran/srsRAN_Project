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

#pragma once

#include "../../../lib/phy/support/prach_buffer_impl.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/file_tensor.h"

namespace srsran {

class prach_buffer_spy : public prach_buffer
{
public:
  struct entry_t {
    unsigned i_port;
    unsigned i_td_occasion;
    unsigned i_fd_occasion;
    unsigned i_symbol;
  };

  prach_buffer_spy() : buffer(0, 0, 0, 0, 0) {}

  prach_buffer_spy(span<cf_t> data_,
                   unsigned   nof_td_occasions,
                   unsigned   nof_fd_occasions,
                   unsigned   nof_symbols,
                   unsigned   sequence_length) :
    buffer(1, nof_td_occasions, nof_fd_occasions, nof_symbols, sequence_length)
  {
    span<const cf_t> data      = data_;
    unsigned         nof_ports = get_max_nof_ports_inner();

    report_fatal_error_if_not(data.size() == nof_td_occasions * nof_fd_occasions * sequence_length,
                              "The symbols data size is not consistent with the symbol size and number of symbols.");

    for (unsigned i_td_occasion = 0; i_td_occasion != nof_td_occasions; ++i_td_occasion) {
      for (unsigned i_fd_occasion = 0; i_fd_occasion != nof_fd_occasions; ++i_fd_occasion) {
        span<const cf_t> occasion_data = data.first(sequence_length);
        data                           = data.last(data.size() - sequence_length);
        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
            srsvec::copy(buffer.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol), occasion_data);
          }
        }
      }
    }
  }

  unsigned get_max_nof_ports() const override { return get_max_nof_ports_inner(); }

  unsigned get_max_nof_td_occasions() const override
  {
    ++count_get_max_nof_td_occasions;
    return buffer.get_max_nof_td_occasions();
  }

  unsigned get_max_nof_fd_occasions() const override
  {
    ++count_get_max_nof_fd_occasions;
    return buffer.get_max_nof_fd_occasions();
  }

  unsigned get_max_nof_symbols() const override
  {
    ++count_get_max_nof_symbols;
    return buffer.get_max_nof_symbols();
  }

  unsigned get_sequence_length() const override
  {
    ++count_get_sequence_length;
    return buffer.get_sequence_length();
  }

  span<cf_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) override
  {
    get_symbol_entries.emplace_back();
    entry_t& entry      = get_symbol_entries.back();
    entry.i_port        = i_port;
    entry.i_td_occasion = i_td_occasion;
    entry.i_fd_occasion = i_fd_occasion;
    entry.i_symbol      = i_symbol;
    return buffer.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol);
  }

  span<const cf_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override
  {
    get_symbol_const_entries.emplace_back();
    entry_t& entry      = get_symbol_const_entries.back();
    entry.i_port        = i_port;
    entry.i_td_occasion = i_td_occasion;
    entry.i_fd_occasion = i_fd_occasion;
    entry.i_symbol      = i_symbol;
    return buffer.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol);
  }

  unsigned get_total_count() const
  {
    return count_get_max_nof_ports + count_get_max_nof_td_occasions + count_get_max_nof_fd_occasions +
           count_get_max_nof_symbols + count_get_sequence_length + get_symbol_entries.size() +
           get_symbol_const_entries.size();
  }

  const std::vector<entry_t>& get_get_symbol_entries() { return get_symbol_entries; };
  const std::vector<entry_t>& get_get_symbol_const_entries() { return get_symbol_const_entries; };

  void clear()
  {
    count_get_max_nof_ports        = 0;
    count_get_max_nof_td_occasions = 0;
    count_get_max_nof_fd_occasions = 0;
    count_get_max_nof_symbols      = 0;
    count_get_sequence_length      = 0;
  }

private:
  /// Underlying buffer.
  prach_buffer_impl buffer;

  mutable unsigned             count_get_max_nof_ports        = 0;
  mutable unsigned             count_get_max_nof_td_occasions = 0;
  mutable unsigned             count_get_max_nof_fd_occasions = 0;
  mutable unsigned             count_get_max_nof_symbols      = 0;
  mutable unsigned             count_get_sequence_length      = 0;
  std::vector<entry_t>         get_symbol_entries;
  mutable std::vector<entry_t> get_symbol_const_entries;

  unsigned get_max_nof_ports_inner() const
  {
    ++count_get_max_nof_ports;
    return buffer.get_max_nof_ports();
  }
};

/// Describes a generic resource grid implementation
class prach_buffer_tensor : public prach_buffer
{
public:
  /// Data storage dimensions.
  enum class dims : unsigned {
    re          = 0,
    symbol      = 1,
    fd_occasion = 2,
    td_occasion = 3,
    port        = 4,
    count       = 5,
  };

  using storage_type = tensor<static_cast<std::underlying_type_t<dims>>(dims::count), cf_t, dims>;

  /// Creates a PRACH buffer from the maximum parameters depending on the configuration.
  explicit prach_buffer_tensor(storage_type& data_) : data(data_) {}

  // See interface for documentation.
  unsigned get_max_nof_ports() const override { return data.get_dimension_size(dims::port); }

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

private:
  /// Data storage.
  storage_type& data;
};

} // namespace srsran
