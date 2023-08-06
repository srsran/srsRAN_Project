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

#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/resource_block.h"
#include <mutex>

namespace srsran {
namespace ofh {

/// Uplink PRACH context.
class ul_prach_context
{
  /// PRACH buffer writer statistics.
  struct prach_buffer_writer_stats {
    /// Number of REs of the grid.
    unsigned buffer_nof_re = 0;
    /// Number of REs written indexed by port.
    static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> re_written;

    /// Returns true when all the RE for all the ports have been written.
    explicit operator bool() const noexcept { return buffer_nof_re != 0; }

    /// Returns true when all the REs for the current symbol have been written.
    bool have_all_res_been_written() const
    {
      return std::all_of(re_written.begin(), re_written.end(), [this](unsigned port_re_written) {
        return port_re_written == this->buffer_nof_re;
      });
    }
  };

public:
  /// Default constructor.
  ul_prach_context() = default;

  /// Constructs an uplink PRACH context with the given PRACH buffer and PRACH buffer context.
  ul_prach_context(const prach_buffer_context& context_, prach_buffer& buffer_, unsigned nof_ports_) :
    context(context_), buffer(&buffer_), nof_symbols(get_preamble_duration(context.format)), nof_ports(nof_ports_)
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
    buffer_stats.buffer_nof_re = (preamble_info.sequence_length * nof_symbols);
    buffer_stats.re_written    = static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC>(nof_ports, 0);
  }

  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return buffer == nullptr; }

  /// Returns the number of REs of one PRACH repetition or zero if no PRACH buffer is associated with this context.
  unsigned get_prach_nof_re() const { return empty() ? 0U : (preamble_info.sequence_length + freq_mapping_info.k_bar); }

  /// Returns the number of symbols used by the PRACH associated with the stored context.
  unsigned get_prach_nof_symbols() const { return empty() ? 0U : nof_symbols; }

  /// Writes the given IQ buffer corresponding to the given symbol and port and notifies that an uplink PRACH buffer is
  /// ready when all the PRBs for all the symbols and ports have been written in the buffer.
  bool update_buffer_and_notify(unsigned                   port,
                                unsigned                   symbol,
                                span<const cf_t>           iq_buffer,
                                uplane_rx_symbol_notifier& notifier)
  {
    srsran_assert(buffer, "No valid PRACH buffer in the context");
    srsran_assert(symbol < nof_symbols, "Invalid symbol index");

    if (!buffer_stats) {
      return false;
    }

    // Skip writting if the given port does not fit in the PRACH buffer.
    if (port >= nof_ports) {
      return false;
    }

    // Update the buffer.
    span<cf_t> prach_out_buffer =
        buffer->get_symbol(port, context.nof_fd_occasions - 1, context.nof_td_occasions - 1, symbol);

    unsigned nof_re_to_prach_data = freq_mapping_info.k_bar;
    // Grab the data.
    span<const cf_t> prach_in_data = iq_buffer.subspan(nof_re_to_prach_data, preamble_info.sequence_length);
    std::copy(prach_in_data.begin(), prach_in_data.end(), prach_out_buffer.begin());
    // Update statistics.
    buffer_stats.re_written[port] += prach_in_data.size();

    // Notify when PRACH buffer is ready.
    if (!buffer_stats || !buffer_stats.have_all_res_been_written()) {
      return false;
    }
    notifier.on_new_prach_window_data(context, *buffer);

    // Mark the symbol as sent.
    buffer_stats = {};
    return true;
  }

private:
  /// PRACH buffer context.
  prach_buffer_context context;
  /// PRACH buffer.
  prach_buffer* buffer = nullptr;
  /// Statistic of written data.
  prach_buffer_writer_stats buffer_stats;
  /// Preamble info of the PRACH associated with the stored context.
  prach_preamble_information preamble_info;
  /// Stored PRACH frequency mapping.
  prach_frequency_mapping_information freq_mapping_info;
  /// Number of OFDM symbols used by the stored PRACH.
  unsigned nof_symbols;
  /// Number of PRACH ports.
  unsigned nof_ports;
};

/// Uplink slot context.
class ul_slot_context
{
  /// Resource grid writer statistics.
  struct resource_grid_writer_stats {
    /// Number of REs of the grid.
    unsigned grid_nof_re = 0;
    /// Number of REs written indexed by port.
    static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> re_written;

    /// Returns true when all the RE for all the ports have been written.
    explicit operator bool() const noexcept { return grid_nof_re != 0; }

    /// Returns true when all the REs for the current symbol have been written.
    bool have_all_prbs_been_written() const
    {
      return std::all_of(re_written.begin(), re_written.end(), [&](unsigned port_re_written) {
        return port_re_written == grid_nof_re;
      });
    }
  };

public:
  /// Default constructor.
  ul_slot_context() = default;

  /// Constructs an uplink slot context with the given resource grid and resource grid context.
  ul_slot_context(const resource_grid_context& context_, resource_grid& grid_) : context(context_), grid(&grid_)
  {
    for (unsigned symbol_id = 0, e = grid_.get_writer().get_nof_symbols(); symbol_id != e; ++symbol_id) {
      rg_stats[symbol_id].grid_nof_re = grid_.get_writer().get_nof_subc();
      rg_stats[symbol_id].re_written =
          static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC>(grid_.get_writer().get_nof_ports(), 0);
    }
  }

  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return grid == nullptr; }

  /// Returns the number of PRBs of the context grid or zero if no grid was configured for this context.
  unsigned get_grid_nof_prbs() const
  {
    return (grid) ? (grid->get_writer().get_nof_subc() / NOF_SUBCARRIERS_PER_RB) : 0U;
  }

  /// Writes the given RE IQ buffer into the given symbol, port and start RE and notifies that an uplink symbol has been
  /// completed when all the PRBs for all the ports have been written in the grid.
  void update_grid_and_notify(unsigned                   port,
                              unsigned                   symbol,
                              unsigned                   start_re,
                              span<const cf_t>           re_iq_buffer,
                              uplane_rx_symbol_notifier& notifier)
  {
    srsran_assert(grid, "Invalid resource grid");

    // Update the grid.
    write_grid(port, symbol, start_re, re_iq_buffer);

    // Notify the received symbol when it is complete.
    notify_symbol_when_complete(symbol, notifier);
  }

  /// Notify using the given notifier.
  void notify_symbol(unsigned symbol, uplane_rx_symbol_notifier& notifier)
  {
    if (!rg_stats[symbol]) {
      return;
    }

    // Create the notification context.
    uplane_rx_symbol_context notifier_context;
    notifier_context.symbol = symbol;
    notifier_context.slot   = context.slot;

    // Notify.
    notifier.on_new_uplink_symbol(notifier_context, grid->get_reader());

    // Mark the symbol as sent.
    rg_stats[symbol] = {};
  }

private:
  /// Writes the given RE IQ buffer into the grid using the port, symbol and start RE.
  void write_grid(unsigned port, unsigned symbol, unsigned start_re, span<const cf_t> re_iq_buffer)
  {
    // Symbol is not valid for this grid or already been sent.
    if (!rg_stats[symbol]) {
      return;
    }

    // Skip writing if the given port does not fit in the grid.
    if (port >= grid->get_writer().get_nof_ports()) {
      return;
    }

    grid->get_writer().put(port, symbol, start_re, re_iq_buffer);
    rg_stats[symbol].re_written[port] += re_iq_buffer.size();
  }

  /// Notify using the given notifier when all the REs for all the ports of the given symbol have been written.
  void notify_symbol_when_complete(unsigned symbol, uplane_rx_symbol_notifier& notifier)
  {
    if (!rg_stats[symbol] || !rg_stats[symbol].have_all_prbs_been_written()) {
      return;
    }

    // Create the notification context.
    uplane_rx_symbol_context notifier_context;
    notifier_context.symbol = symbol;
    notifier_context.slot   = context.slot;
    notifier_context.sector = context.sector;

    // Notify.
    notifier.on_new_uplink_symbol(notifier_context, grid->get_reader());

    // Mark the symbol as sent.
    rg_stats[symbol] = {};
  }

private:
  resource_grid_context                                      context;
  resource_grid*                                             grid     = nullptr;
  std::array<resource_grid_writer_stats, MAX_NSYMB_PER_SLOT> rg_stats = {};
};

/// Uplink context repository.
template <typename T>
class uplink_context_repository
{
  /// System frame number maximum value in this repository.
  static constexpr unsigned SFN_MAX_VALUE = 1U << 8;

  std::vector<T> buffer;
  const unsigned size;
  //: TODO: make this lock free
  mutable std::mutex mutex;

  /// Returns the entry of the repository for the given slot.
  T& entry(slot_point slot)
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % size;
    return buffer[index];
  }

  /// Returns the entry of the repository for the given slot.
  const T& entry(slot_point slot) const
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % size;
    return buffer[index];
  }

public:
  explicit uplink_context_repository(unsigned size_) : buffer(size_), size(size_) {}

  /// Adds the given entry to the repository at slot.
  void add(slot_point slot, const T& new_entry)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot) = new_entry;
  }

  /// Function to write the grid for the uplink slot context.
  template <typename X = T>
  typename std::enable_if<std::is_same<ul_slot_context, X>::value>::type
  update_grid_and_notify(slot_point                 slot,
                         unsigned                   port,
                         unsigned                   symbol,
                         unsigned                   start_re,
                         span<const cf_t>           re_iq_buffer,
                         uplane_rx_symbol_notifier& notifier)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot).update_grid_and_notify(port, symbol, start_re, re_iq_buffer, notifier);
  }

  /// Function to write the uplink PRACH buffer. Returns True in case notifier was called, false otherwise.
  template <typename X = T>
  typename std::enable_if<std::is_same<ul_prach_context, X>::value, bool>::type
  update_buffer_and_notify(slot_point                 slot,
                           unsigned                   port,
                           unsigned                   symbol,
                           span<const cf_t>           iq_buffer,
                           uplane_rx_symbol_notifier& notifier)
  {
    std::lock_guard<std::mutex> lock(mutex);
    return entry(slot).update_buffer_and_notify(port, symbol, iq_buffer, notifier);
  }

  /// Returns the entry of the repository for the given slot.
  T get(slot_point slot) const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return entry(slot);
  }
};

} // namespace ofh
} // namespace srsran
