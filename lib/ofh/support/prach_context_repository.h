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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/expected.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include <mutex>
#include <numeric>

namespace srsran {
namespace ofh {

///  PRACH context.
class prach_context
{
  /// PRACH buffer writer statistics.
  struct prach_buffer_writer_stats {
    prach_buffer_writer_stats(unsigned nof_ports, size_t nof_res) :
      re_written(nof_ports, bounded_bitset<prach_constants::LONG_SEQUENCE_LENGTH>(nof_res))
    {
    }

    /// Number of REs written indexed by port.
    static_vector<bounded_bitset<prach_constants::LONG_SEQUENCE_LENGTH>, MAX_NOF_SUPPORTED_EAXC> re_written;

    /// Returns true when all the REs for the current symbol have been written.
    bool have_all_res_been_written() const
    {
      return std::all_of(re_written.begin(), re_written.end(), [&](const auto& port) { return port.all(); });
    }
  };

public:
  struct prach_context_information {
    /// PRACH buffer context.
    prach_buffer_context context;
    /// PRACH buffer.
    prach_buffer* buffer = nullptr;
  };

  /// Default constructor.
  prach_context() = default;

  /// Constructs an uplink PRACH context with the given PRACH buffer and PRACH buffer context.
  prach_context(const prach_buffer_context& context, prach_buffer& buffer, unsigned nof_ports_) :
    nof_ports(nof_ports_), context_info({context, &buffer}), nof_symbols(get_preamble_duration(context.format))
  {
    srsran_assert(context.nof_fd_occasions == 1, "Only supporting one frequency domain occasion");
    srsran_assert(context.nof_td_occasions == 1, "Only supporting one time domain occasion");

    // Get preamble information.
    preamble_info =
        is_long_preamble(context.format)
            ? get_prach_preamble_long_info(context.format)
            : get_prach_preamble_short_info(context.format, to_ra_subcarrier_spacing(context.pusch_scs), true);

    freq_mapping_info = prach_frequency_mapping_get(preamble_info.scs, context.pusch_scs);

    if (!nof_symbols) {
      nof_symbols = 1;
    }
    // Initialize statistic.
    for (unsigned i = 0; i != nof_symbols; ++i) {
      buffer_stats.push_back({nof_ports, preamble_info.sequence_length});
    }
  }

  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return context_info.buffer == nullptr; }

  /// Returns the number of REs of one PRACH repetition or zero if no PRACH buffer is associated with this context.
  unsigned get_prach_nof_re() const { return empty() ? 0U : preamble_info.sequence_length; }

  /// Returns the offset to the first resource element.
  unsigned get_prach_offset_to_first_re() const { return freq_mapping_info.k_bar; }

  /// Gets the maximum number of ports supported in PRACH buffer.
  unsigned get_max_nof_ports() const { return empty() ? 0U : nof_ports; }

  /// Returns a span of bitmaps that indicate the REs that have been written for the given symbol. Each element of the
  /// span corresponds to a port.
  span<const bounded_bitset<prach_constants::LONG_SEQUENCE_LENGTH>> get_symbol_re_written(unsigned symbol) const
  {
    srsran_assert(symbol < nof_symbols, "Invalid symbol index");
    return buffer_stats[symbol].re_written;
  }

  /// Writes the given IQ buffer corresponding to the given symbol and port.
  void write_iq(unsigned port, unsigned symbol, unsigned re_start, span<const cf_t> iq_buffer)
  {
    srsran_assert(context_info.buffer, "No valid PRACH buffer in the context");
    srsran_assert(symbol < nof_symbols, "Invalid symbol index");

    // Skip writing if the given port does not fit in the PRACH buffer.
    if (port >= nof_ports) {
      return;
    }

    // Update the buffer.
    span<cf_t> prach_out_buffer = context_info.buffer->get_symbol(
        port, context_info.context.nof_fd_occasions - 1, context_info.context.nof_td_occasions - 1, symbol);

    srsran_assert(prach_out_buffer.last(prach_out_buffer.size() - re_start).size() >= iq_buffer.size(),
                  "Invalid IQ data buffer size to copy as it does not fit into the PRACH buffer");

    std::copy(iq_buffer.begin(), iq_buffer.end(), prach_out_buffer.begin() + re_start);

    // Update statistics.
    buffer_stats[symbol].re_written[port].fill(re_start, re_start + iq_buffer.size());
  }

  /// Tries to get a complete PRACH buffer. A PRACH buffer is considered completed when all the PRBs for all the ports
  /// have been written.
  expected<prach_context_information> try_getting_complete_prach_buffer() const
  {
    if (!context_info.buffer) {
      return default_error_t({});
    }

    if (!std::all_of(buffer_stats.begin(), buffer_stats.end(), [&](const auto& symbol) {
          return symbol.have_all_res_been_written();
        })) {
      return default_error_t({});
    }

    return {context_info};
  }

private:
  /// Number of ports.
  unsigned nof_ports;
  /// PRACH context information
  prach_context_information context_info;
  /// Statistic of written data.
  static_vector<prach_buffer_writer_stats, prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS> buffer_stats;
  /// Preamble info of the PRACH associated with the stored context.
  prach_preamble_information preamble_info;
  /// Stored PRACH frequency mapping.
  prach_frequency_mapping_information freq_mapping_info;
  /// Number of OFDM symbols used by the stored PRACH.
  unsigned nof_symbols;
};

/// PRACH context repository.
class prach_context_repository
{
  /// System frame number maximum value in this repository.
  static constexpr unsigned SFN_MAX_VALUE = 1U << 8;

  std::vector<prach_context> buffer;
  //: TODO: make this lock free
  mutable std::mutex mutex;

  /// Returns the entry of the repository for the given slot.
  prach_context& entry(slot_point slot)
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % buffer.size();
    return buffer[index];
  }

  /// Returns the entry of the repository for the given slot.
  const prach_context& entry(slot_point slot) const
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % buffer.size();
    return buffer[index];
  }

public:
  explicit prach_context_repository(unsigned size_) : buffer(size_) {}

  /// Adds the given entry to the repository at slot.
  void add(const prach_buffer_context& context,
           prach_buffer&               buffer_,
           unsigned                    nof_ports = 1,
           slot_point                  slot      = slot_point())
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot.valid() ? slot : context.slot) = prach_context(context, buffer_, nof_ports);
  }

  /// Function to write the uplink PRACH buffer.
  void write_iq(slot_point slot, unsigned port, unsigned symbol, unsigned re_start, span<const cf_t> iq_buffer)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot).write_iq(port, symbol, re_start, iq_buffer);
  }

  /// Returns the entry of the repository for the given slot.
  prach_context get(slot_point slot) const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return entry(slot);
  }

  /// Clears the given slot entry.
  void clear(slot_point slot)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot) = {};
  }
};

} // namespace ofh
} // namespace srsran
