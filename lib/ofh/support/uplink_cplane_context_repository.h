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

#include "srsran/adt/expected.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ofh_cplane_message_properties.h"
#include <mutex>
#include <vector>

namespace srsran {
namespace ofh {

/// Uplink Control-Plane context repository.
class uplink_cplane_context_repository
{
  /// Maximun number of supported Control-Plane contexts per slot (uplink + PRACH).
  static constexpr unsigned MAX_NOF_SUPPORTED_CP_CONTEXT_PER_SLOT = 2;

  /// System frame number maximum value in this repository.
  static constexpr unsigned SFN_MAX_VALUE = 1U << 8;

  using repo_entry = static_vector<cplane_section_type1_parameters, MAX_NOF_SUPPORTED_CP_CONTEXT_PER_SLOT>;

  std::vector<repo_entry> repo;
  unsigned                size = 0;
  //: TODO: make this lock free
  mutable std::mutex mutex;

  /// Returns the entry of the repository for the given slot.
  repo_entry& entry(slot_point slot)
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % size;
    return repo[index];
  }

  /// Returns the entry of the repository for the given slot.
  const repo_entry& entry(slot_point slot) const
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % size;
    return repo[index];
  }

public:
  explicit uplink_cplane_context_repository(unsigned size_) : repo(size_), size(size_) {}

  /// Add the given context to the repo at the given slot.
  void add(slot_point slot, const cplane_section_type1_parameters& context)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot).push_back(context);
  }

  /// Returns a context that matches the given slot, symbol and filter index or an error if it does not exist in the
  /// repository.
  expected<cplane_section_type1_parameters> get(slot_point slot, unsigned symbol, filter_index_type filter_index) const
  {
    std::lock_guard<std::mutex> lock(mutex);

    const repo_entry& ent = entry(slot);
    if (ent.empty()) {
      return default_error_t{};
    }

    for (const auto& context : ent) {
      if (symbol >= context.radio_hdr.start_symbol &&
          symbol < (context.radio_hdr.start_symbol + context.section_fields.common_fields.nof_symbols) &&
          filter_index == context.radio_hdr.filter_index) {
        return context;
      }
    }

    return default_error_t{};
  }

  /// Clears the entry at the given slot.
  void clear_slot(slot_point slot)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot).clear();
  }
};

} // namespace ofh
} // namespace srsran
