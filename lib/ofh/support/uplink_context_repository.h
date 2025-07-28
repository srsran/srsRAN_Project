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

#include "context_repository_helpers.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/resource_block.h"
#include "srsran/srslog/logger.h"
#include "srsran/srsvec/copy.h"
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
    shared_resource_grid  grid;
  };

  /// Default constructor.
  uplink_context() = default;

  uplink_context copy() const
  {
    uplink_context context;
    context.symbol       = symbol;
    context.grid.context = grid.context;
    context.grid.grid    = grid.grid.copy();
    context.re_written   = re_written;
    return context;
  }

  /// Constructs an uplink slot context with the given resource grid and resource grid context.
  uplink_context(unsigned symbol_, const resource_grid_context& context_, const shared_resource_grid& grid_) :
    symbol(symbol_), grid({context_, grid_.copy()})
  {
    const resource_grid_reader& reader = grid.grid->get_reader();

    re_written = static_vector<bounded_bitset<MAX_NOF_PRBS * NRE>, MAX_NOF_SUPPORTED_EAXC>(
        reader.get_nof_ports(), bounded_bitset<MAX_NOF_PRBS * NRE>(size_t(reader.get_nof_subc())));
  }

  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return !grid.grid.is_valid(); }

  /// Returns the number of PRBs of the context grid or zero if no grid was configured for this context.
  unsigned get_grid_nof_prbs() const
  {
    return (grid.grid) ? (grid.grid.get_reader().get_nof_subc() / NOF_SUBCARRIERS_PER_RB) : 0U;
  }

  /// Returns the resource grid context.
  const resource_grid_context& get_grid_context() const { return grid.context; }

  /// Returns a span of bitmaps that indicate the REs that have been written for the given symbol. Each element of the
  /// span corresponds to a port.
  span<const bounded_bitset<MAX_NOF_PRBS * NRE>> get_re_written_mask() const { return re_written; }

  /// Writes the given RE IQ buffer into the port and start RE.
  void write_grid(unsigned port, unsigned start_re, span<const cbf16_t> re_iq_buffer)
  {
    srsran_assert(grid.grid, "Invalid resource grid");
    resource_grid_writer& writer = grid.grid->get_writer();

    // Skip writing if the given port does not fit in the grid.
    if (port >= writer.get_nof_ports()) {
      return;
    }
    span<cbf16_t> grid_view = grid.grid->get_writer().get_view(port, symbol).subspan(start_re, re_iq_buffer.size());
    srsvec::copy(grid_view, re_iq_buffer);
    re_written[port].fill(start_re, start_re + re_iq_buffer.size());
  }

  /// Tries to get a complete resource grid. A resource grid is considered completed when all the PRBs for all the ports
  /// have been written.
  expected<uplink_context_resource_grid_info> try_getting_complete_resource_grid() const
  {
    if (!grid.grid) {
      return make_unexpected(default_error_t{});
    }

    if (!have_all_prbs_been_written()) {
      return make_unexpected(default_error_t{});
    }

    return uplink_context_resource_grid_info{grid.context, grid.grid.copy()};
  }

  /// Returns the context grid information.
  const uplink_context_resource_grid_info& get_uplink_context_resource_grid_info() const { return grid; }

  /// Gets the context grid information and clears it.
  uplink_context_resource_grid_info pop_uplink_context_resource_grid_info() { return std::move(grid); }

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
  using queue_type =
      concurrent_queue<unique_task, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  queue_type                                                  pending_context_to_add;
  std::vector<std::array<uplink_context, MAX_NSYMB_PER_SLOT>> buffer;
  //: TODO: make this lock free
  mutable std::mutex mutex;

  // :TODO: temporal solution to not allow adding more entries when the repo is not running. This mechanism should be
  // changed when the start/stop procedure is implemented (probably not needed in that case as no grid will arrive after
  // commanding the stop).
  bool is_running = true;

  /// Returns the entry of the repository for the given slot and symbol.
  uplink_context& entry(slot_point slot, unsigned symbol)
  {
    srsran_assert(symbol < MAX_NSYMB_PER_SLOT, "Invalid symbol index '{}'", symbol);

    unsigned index = calculate_repository_index(slot, buffer.size());
    return buffer[index][symbol];
  }

  /// Returns the entry of the repository for the given slot and symbol.
  const uplink_context& entry(slot_point slot, unsigned symbol) const
  {
    srsran_assert(symbol < MAX_NSYMB_PER_SLOT, "Invalid symbol index '{}'", symbol);

    unsigned index = calculate_repository_index(slot, buffer.size());
    return buffer[index][symbol];
  }

public:
  explicit uplink_context_repository(unsigned size_) : pending_context_to_add(size_), buffer(size_) {}

  /// Adds the given entry to the repository at slot.
  void add(const resource_grid_context& context,
           const shared_resource_grid&  grid,
           const ofdm_symbol_range&     symbol_range,
           srslog::basic_logger&        logger)
  {
    // Do not add entry if repo is not running.
    if (!is_running) {
      return;
    }

    if (!pending_context_to_add.try_push([context, rg = grid.copy(), symbol_range, this]() {
          std::lock_guard<std::mutex> lock(mutex);
          for (unsigned symbol_id = symbol_range.start(), symbol_end = symbol_range.stop(); symbol_id != symbol_end;
               ++symbol_id) {
            entry(context.slot, symbol_id) = uplink_context(symbol_id, context, rg);
          }
        })) {
      logger.warning("Failed to enqueue task to add the uplink context to the repository");
    }
  }

  /// Process the enqueued contexts to the repository.
  void process_pending_contexts()
  {
    unique_task task;
    while (pending_context_to_add.try_pop(task)) {
      task();
    }
  }

  /// Writes to the grid at the given slot, port, symbol and start resource element the given IQ buffer.
  void write_grid(slot_point slot, unsigned port, unsigned symbol, unsigned start_re, span<const cbf16_t> re_iq_buffer)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot, symbol).write_grid(port, start_re, re_iq_buffer);
  }

  /// Returns the entry of the repository for the given slot and symbol.
  uplink_context get(slot_point slot, unsigned symbol) const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return entry(slot, symbol).copy();
  }

  /// \brief Tries to pop a complete resource grid for the given slot and symbol.
  ///
  /// A resource grid is considered completed when all the PRBs for all the ports have been written.
  expected<uplink_context::uplink_context_resource_grid_info> try_popping_complete_resource_grid_symbol(slot_point slot,
                                                                                                        unsigned symbol)
  {
    std::lock_guard<std::mutex> lock(mutex);
    auto                        result = entry(slot, symbol).try_getting_complete_resource_grid();

    // Symbol is complete or exists. Clear the context.
    if (result) {
      entry(slot, symbol) = {};
    }

    return result;
  }

  /// Pops a resource grid for the given slot and symbol.
  expected<uplink_context::uplink_context_resource_grid_info> pop_resource_grid_symbol(slot_point slot, unsigned symbol)
  {
    std::lock_guard<std::mutex> lock(mutex);

    auto& result = entry(slot, symbol);

    // Symbol does not exist. Do nothing.
    if (result.empty()) {
      return make_unexpected(default_error_t{});
    }

    // Pop and clear the slot/symbol information.
    uplink_context::uplink_context_resource_grid_info info = result.pop_uplink_context_resource_grid_info();
    return info;
  }

  /// Clears the repository entry for the given slot and symbol.
  void clear(slot_point slot, unsigned symbol)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot, symbol) = {};
  }

  /// \brief Clears the whole repository.
  void clear()
  {
    std::lock_guard<std::mutex> lock(mutex);

    is_running = false;
    for (auto& elem : buffer) {
      for (auto& symbol : elem) {
        symbol = {};
      }
    }
  }
};

} // namespace ofh
} // namespace srsran
