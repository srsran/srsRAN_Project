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
#include "srsran/ran/resource_block.h"
#include <mutex>

namespace srsran {
namespace ofh {

/// Uplink PRACH context.
struct ul_prach_context {
  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return buffer == nullptr; }

  /// PRACH buffer context.
  prach_buffer_context context;
  /// PRACH buffer.
  prach_buffer* buffer = nullptr;
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

  /// Returns the entry of the repository for the given slot.
  T get(slot_point slot) const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return entry(slot);
  }

  /// Clears the repository entry at slot.
  void clear(slot_point slot)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot) = T();
  }
};

} // namespace ofh
} // namespace srsran
