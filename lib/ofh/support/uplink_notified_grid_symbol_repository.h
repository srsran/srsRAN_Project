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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include <atomic>
#include <optional>
#include <vector>

namespace srsran {
namespace ofh {

/// Uplink notified grid symbol repository. Stores the symbols that have been notified to upper layers for a given slot.
class uplink_notified_grid_symbol_repository
{
  /// Starting from bit 16, it codifies if the symbol is valid (1) or not valid (0).
  static constexpr unsigned VALID_SYMBOL_BITMASK_START_BIT = 16;
  /// Cyclic prefix bit. 1 for extended, 0 for normal.
  static constexpr unsigned EXTENDED_CP_BIT = 14;
  /// Used entry bit. 1 for slot used, 0 slot not used.
  static constexpr unsigned USED_SLOT_BIT                    = 15;
  static constexpr uint32_t USED_SLOT_BITMAP                 = 1u << USED_SLOT_BIT;
  static constexpr uint32_t EXTENDED_CP_BITMAP               = 1u << EXTENDED_CP_BIT;
  static constexpr uint32_t ALL_SYMBOLS_NOTIFIED_CP_NORMAL   = (1u << get_nsymb_per_slot(cyclic_prefix::NORMAL)) - 1;
  static constexpr uint32_t ALL_SYMBOLS_NOTIFIED_CP_EXTENDED = (1u << get_nsymb_per_slot(cyclic_prefix::EXTENDED)) - 1;

  using repo_entry = std::atomic<uint32_t>;
  std::vector<repo_entry> repo;

  /// Returns the entry for the given slot.
  std::atomic<uint32_t>& get_entry(slot_point slot) { return repo[slot.system_slot() % repo.size()]; }

  /// Returns true if the slot is used, otherwise false.
  static constexpr bool is_slot_used(uint32_t value) { return value & USED_SLOT_BITMAP; }

  /// Returns true if all symbols have been notified, otherwise false.
  static constexpr bool are_all_symbols_notified(uint32_t value)
  {
    if (value & EXTENDED_CP_BITMAP) {
      return (value & ALL_SYMBOLS_NOTIFIED_CP_EXTENDED) == ALL_SYMBOLS_NOTIFIED_CP_EXTENDED;
    }

    return (value & ALL_SYMBOLS_NOTIFIED_CP_NORMAL) == ALL_SYMBOLS_NOTIFIED_CP_NORMAL;
  }

  /// Returns true if any symbol smaller than the given symbol has not been notified, otherwise false.
  static constexpr bool is_any_previous_symbol_missed(uint32_t value, unsigned symbol)
  {
    for (unsigned i = 0, e = symbol; i != e; ++i) {
      if (((value >> i) & 1) == 0) {
        return true;
      }
    }

    return false;
  }

  /// Returns the maximum number of symbols of the given value.
  static constexpr unsigned get_nof_symbols(uint32_t value)
  {
    return get_nsymb_per_slot(static_cast<cyclic_prefix::options>((value & EXTENDED_CP_BITMAP) >> EXTENDED_CP_BIT));
  }

  /// Returns the number of symbols to notify starting from the given symbol.
  static constexpr unsigned get_nof_symbols_to_notify(uint32_t value, unsigned symbol)
  {
    unsigned nof_symbols = get_nof_symbols(value);

    value = value >> symbol;

    // :TODO: use ctz when the public API is implemented.
    // Start in one to count for symbol and check the higher symbols.
    for (unsigned i = 1, e = nof_symbols - symbol; i != e; ++i) {
      if (((value >> i) & 1) == 0) {
        return i;
      }
    }

    // At this point all the upper symbols are received, notify all of them.
    return nof_symbols - symbol;
  }

  /// Clears the slot by setting the entry to not used.
  void clear_slot(slot_point slot) { get_entry(slot).store(0, std::memory_order_relaxed); }

public:
  struct results {
    results() : valid_symbols(MAX_NSYMB_PER_SLOT) {}
    interval<unsigned>                 symbols;
    bounded_bitset<MAX_NSYMB_PER_SLOT> valid_symbols;
  };

  explicit uplink_notified_grid_symbol_repository(unsigned size) : repo(size) {}

  /// Adds an entry to the repository for the given slot and start_symbol. Usually start symbol is 0 for the whole grid,
  /// but can be modified.
  void add(slot_point slot, unsigned start_symbol = 0U, cyclic_prefix cp = cyclic_prefix::NORMAL)
  {
    srsran_assert(start_symbol < MAX_NSYMB_PER_SLOT, "Invalid start symbol '{}'", start_symbol);

    uint32_t value = USED_SLOT_BITMAP;
    if (cp == cyclic_prefix::EXTENDED) {
      value |= EXTENDED_CP_BITMAP;
    }

    // Mark the bits as already sent for symbols < start_symbol.
    value |= static_cast<uint32_t>((uint32_t(1u) << start_symbol) - 1);

    get_entry(slot).store(value, std::memory_order_relaxed);
  }

  /// Updates grid received symbol for the given slot and returns a list of symbols to notify.
  std::optional<results> update_rx_symbol_and_compute_symbols_to_notify(slot_point slot, unsigned symbol, bool is_valid)
  {
    srsran_assert(symbol < MAX_NSYMB_PER_SLOT, "Invalid symbol '{}'", symbol);

    uint32_t bitmask = 0;
    bitmask |= uint32_t(1u) << symbol;
    // Set the bitmask validity.
    if (is_valid) {
      bitmask |= uint32_t(1u) << (symbol + VALID_SYMBOL_BITMASK_START_BIT);
    }

    uint32_t value = get_entry(slot).fetch_or(bitmask, std::memory_order_relaxed);

    if (!is_slot_used(value)) {
      return std::nullopt;
    }

    if (is_any_previous_symbol_missed(value, symbol)) {
      return std::nullopt;
    }

    unsigned nof_symbols_to_notify = get_nof_symbols_to_notify(value, symbol);

    value |= bitmask;

    if (are_all_symbols_notified(value)) {
      clear_slot(slot);
    }

    results output;
    output.symbols = {symbol, symbol + nof_symbols_to_notify};
    output.valid_symbols.from_uint64(uint64_t(value >> VALID_SYMBOL_BITMASK_START_BIT));

    return std::make_optional<results>(output);
  }
};

} // namespace ofh
} // namespace srsran
