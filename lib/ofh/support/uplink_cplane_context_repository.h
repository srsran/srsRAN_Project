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
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_cplane_message_properties.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include <array>
#include <atomic>
#include <vector>

namespace srsran {
namespace ofh {

/// Uplink Control-Plane context.
struct ul_cplane_context {
  /// Filter index.
  filter_index_type filter_index;
  /// Start symbol identifier.
  uint8_t start_symbol;
  /// Starting PRB of data section.
  uint16_t prb_start;
  /// Number of contiguous PRBs per data section.
  uint16_t nof_prb;
  /// Number of symbols.
  uint8_t nof_symbols;
};

/// Uplink Control-Plane context repository.
class uplink_cplane_context_repository
{
  using repo_entry = std::array<std::atomic<uint64_t>, MAX_SUPPORTED_EAXC_ID_VALUE>;

  /// Repository storage.
  std::vector<repo_entry> repo;

  /// Returns the entry of the repository for the given slot and eAxC.
  std::atomic<uint64_t>& get_entry(slot_point slot, unsigned eaxc)
  {
    unsigned index = calculate_repository_index(slot, repo.size());
    return repo[index][eaxc];
  }

  /// Returns the entry of the repository for the given slot and eAxC.
  const std::atomic<uint64_t>& get_entry(slot_point slot, unsigned eaxc) const
  {
    unsigned index = calculate_repository_index(slot, repo.size());
    return repo[index][eaxc];
  }

  /// Packs the given context.
  static uint64_t pack_context(ul_cplane_context context)
  {
    uint64_t data = 0;
    data |= static_cast<uint8_t>(context.filter_index);
    data |= static_cast<uint64_t>(context.start_symbol) << 8;
    data |= static_cast<uint64_t>(context.prb_start) << 16;
    data |= static_cast<uint64_t>(context.nof_prb) << 32;
    data |= static_cast<uint64_t>(context.nof_symbols) << 48;

    return data;
  }

  /// Unpacks the given packed context.
  static ul_cplane_context unpack_context(uint64_t data)
  {
    return {static_cast<filter_index_type>(data),
            static_cast<uint8_t>(data >> 8),
            static_cast<uint16_t>(data >> 16),
            static_cast<uint16_t>(data >> 32),
            static_cast<uint8_t>(data >> 48)};
  }

public:
  explicit uplink_cplane_context_repository(unsigned size_) : repo(size_)
  {
    static_assert(MAX_PRACH_OCCASIONS_PER_SLOT == 1,
                  "Uplink Control-Plane context repository only supports one context per slot and eAxC");
  }

  /// Add the given context to the repo at the given slot and eAxC.
  void add(slot_point slot, unsigned eaxc, ul_cplane_context new_context)
  {
    auto& entry = get_entry(slot, eaxc);
    entry.store(pack_context(new_context), std::memory_order_relaxed);
  }

  /// Returns a context that matches the given slot and eAxC.
  ul_cplane_context get(slot_point slot, unsigned eaxc) const
  {
    const auto& entry = get_entry(slot, eaxc);
    return unpack_context(entry.load(std::memory_order_relaxed));
  }
};

} // namespace ofh
} // namespace srsran
