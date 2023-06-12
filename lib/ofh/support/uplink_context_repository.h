/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/ran/cyclic_prefix.h"
#include <bitset>
#include <mutex>
#include <vector>

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
  /// Is PRACH notified flag.
  bool is_notified = false;
  /// Number of REs written in the PRACH buffer.
  unsigned nof_re_written = 0;
};

/// Uplink slot context.
struct ul_slot_context {
  /// Returns true if this context is empty, otherwise false.
  bool empty() const { return grid == nullptr; }

  /// Resource grid context.
  resource_grid_context context;
  /// Resource grid.
  resource_grid* grid = nullptr;
  /// Mask that keeps the state of the grid's notified symbols.
  std::bitset<MAX_NSYMB_PER_SLOT> notified_symbols;
  /// Number of PRBs written for each symbol in the grid.
  std::array<unsigned, MAX_NSYMB_PER_SLOT> prb_written = {};
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
    entry(slot) = {};
  }
};

} // namespace ofh
} // namespace srsran
