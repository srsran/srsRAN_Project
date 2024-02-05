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

#include "srsran/adt/expected.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ofh/timing/slot_symbol_point.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/resource_block.h"
#include <mutex>

namespace srsran {
namespace ofh {

/// Uplink context.
class uplink_context
{
public:
  /// Information related to the resource grid stored in the uplink context.
  struct uplink_context_resource_grid_info {
    resource_grid_context context;
    resource_grid*        grid = nullptr;
  };

  /// Default constructor.
  uplink_context() = default;

  /// Constructs an uplink slot context with the given resource grid and resource grid context.
  uplink_context(unsigned symbol_, const resource_grid_context& context_, resource_grid& grid_) :
    symbol(symbol_), grid({context_, &grid_})
  {
    re_written = static_vector<bounded_bitset<MAX_NOF_PRBS * NRE>, MAX_NOF_SUPPORTED_EAXC>(
        grid_.get_writer().get_nof_ports(),
        bounded_bitset<MAX_NOF_PRBS * NRE>(size_t(grid_.get_writer().get_nof_subc())));
  }

  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return grid.grid == nullptr; }

  /// Returns the number of PRBs of the context grid or zero if no grid was configured for this context.
  unsigned get_grid_nof_prbs() const
  {
    return (grid.grid) ? (grid.grid->get_writer().get_nof_subc() / NOF_SUBCARRIERS_PER_RB) : 0U;
  }

  /// Returns the resource grid context.
  const resource_grid_context& get_grid_context() const { return grid.context; }

  /// Returns a span of bitmaps that indicate the REs that have been written for the given symbol. Each element of the
  /// span corresponds to a port.
  span<const bounded_bitset<MAX_NOF_PRBS * NRE>> get_re_written_mask() const { return re_written; }

  /// Writes the given RE IQ buffer into the port and start RE.
  void write_grid(unsigned port, unsigned start_re, span<const cf_t> re_iq_buffer)
  {
    srsran_assert(grid.grid, "Invalid resource grid");

    // Skip writing if the given port does not fit in the grid.
    if (port >= grid.grid->get_writer().get_nof_ports()) {
      return;
    }

    grid.grid->get_writer().put(port, symbol, start_re, re_iq_buffer);
    re_written[port].fill(start_re, start_re + re_iq_buffer.size());
  }

  /// Tries to get a complete resource grid. A resource grid is considered completed when all the PRBs for all the ports
  /// have been written.
  expected<uplink_context_resource_grid_info> try_getting_complete_resource_grid() const
  {
    if (!grid.grid) {
      return default_error_t({});
    }

    if (!have_all_prbs_been_written()) {
      return default_error_t({});
    }

    return {grid};
  }

private:
  /// Returns true when all the REs for the current symbol have been written.
  bool have_all_prbs_been_written() const
  {
    return std::all_of(
        re_written.begin(), re_written.end(), [](const auto& port_re_written) { return port_re_written.all(); });
  }

private:
  unsigned                                                                  symbol;
  uplink_context_resource_grid_info                                         grid;
  static_vector<bounded_bitset<MAX_NOF_PRBS * NRE>, MAX_NOF_SUPPORTED_EAXC> re_written;
};

/// Uplink context repository.
class uplink_context_repository
{
  /// System frame number maximum value in this repository.
  static constexpr unsigned SFN_MAX_VALUE = 1U << 8;

  srslog::basic_logger*                                       logger;
  std::vector<std::array<uplink_context, MAX_NSYMB_PER_SLOT>> buffer;
  //: TODO: make this lock free
  mutable std::mutex mutex;

  /// Returns the entry of the repository for the given slot and symbol.
  uplink_context& entry(slot_point slot, unsigned symbol)
  {
    srsran_assert(symbol < MAX_NSYMB_PER_SLOT, "Invalid symbol index '{}'", symbol);

    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % buffer.size();
    return buffer[index][symbol];
  }

  /// Returns the entry of the repository for the given slot and symbol.
  const uplink_context& entry(slot_point slot, unsigned symbol) const
  {
    srsran_assert(symbol < MAX_NSYMB_PER_SLOT, "Invalid symbol index '{}'", symbol);

    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % buffer.size();
    return buffer[index][symbol];
  }

public:
  explicit uplink_context_repository(unsigned size_, srslog::basic_logger* logger_ = nullptr) :
    logger(logger_), buffer(size_)
  {
  }

  /// Adds the given entry to the repository at slot.
  void add(const resource_grid_context& context, resource_grid& grid)
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (unsigned symbol_id = 0, symbol_end = grid.get_reader().get_nof_symbols(); symbol_id != symbol_end;
         ++symbol_id) {
      if (logger) {
        if (!entry(context.slot, symbol_id).empty()) {
          const resource_grid_context& previous_context = entry(context.slot, symbol_id).get_grid_context();
          logger->warning("Missed incoming User-Plane uplink messages for slot '{}', symbol '{}' and sector#{}",
                          previous_context.slot,
                          symbol_id,
                          previous_context.sector);
        }
      }

      entry(context.slot, symbol_id) = uplink_context(symbol_id, context, grid);
    }
  }

  /// Writes to the grid at the given slot, port, symbol and start resource element the given IQ buffer.
  void write_grid(slot_point slot, unsigned port, unsigned symbol, unsigned start_re, span<const cf_t> re_iq_buffer)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot, symbol).write_grid(port, start_re, re_iq_buffer);
  }

  /// Returns the entry of the repository for the given slot and symbol.
  uplink_context get(slot_point slot, unsigned symbol) const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return entry(slot, symbol);
  }

  /// Clears the repository entry for the given slot and symbol.
  void clear(slot_point slot, unsigned symbol)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot, symbol) = {};
  }
};

} // namespace ofh
} // namespace srsran
