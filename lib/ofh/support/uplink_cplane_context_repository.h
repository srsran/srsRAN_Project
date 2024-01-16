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
#include "srsran/ofh/serdes/ofh_cplane_message_properties.h"
#include <array>
#include <mutex>
#include <vector>

namespace srsran {
namespace ofh {

/// Uplink Control-Plane context.
struct ul_cplane_context {
  /// Control-Plane radio application header.
  cplane_radio_application_header radio_hdr;
  /// Starting PRB of data section.
  uint16_t prb_start;
  /// Number of contiguous PRBs per data section.
  unsigned nof_prb;
  /// Number of symbols.
  uint8_t nof_symbols;
};

/// Uplink Control-Plane context repository.
class uplink_cplane_context_repository
{
  /// System frame number maximum value in this repository.
  static constexpr unsigned SFN_MAX_VALUE = 1U << 8;

  using repo_entry = std::array<ul_cplane_context, MAX_SUPPORTED_EAXC_ID_VALUE>;

  std::vector<repo_entry> repo;
  unsigned                size = 0;
  //: TODO: make this lock free
  mutable std::mutex mutex;

  /// Returns the entry of the repository for the given slot and eAxC.
  ul_cplane_context& entry(slot_point slot, unsigned eaxc)
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % size;
    return repo[index][eaxc];
  }

  /// Returns the entry of the repository for the given slot and eAxC.
  const ul_cplane_context& entry(slot_point slot, unsigned eaxc) const
  {
    slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
    unsigned   index = entry_slot.system_slot() % size;
    return repo[index][eaxc];
  }

public:
  explicit uplink_cplane_context_repository(unsigned size_) : repo(size_), size(size_) {}

  /// Add the given context to the repo at the given slot and eAxC.
  void add(slot_point slot, unsigned eaxc, const ul_cplane_context& context)
  {
    std::lock_guard<std::mutex> lock(mutex);
    entry(slot, eaxc) = context;
  }

  /// Returns a context that matches the given slot, symbol, filter index and eAxC or an error if it does not exist in
  /// the repository.
  expected<ul_cplane_context> get(slot_point slot, unsigned symbol, filter_index_type filter_index, unsigned eaxc) const
  {
    std::lock_guard<std::mutex> lock(mutex);

    const ul_cplane_context& cp_param = entry(slot, eaxc);
    if (symbol >= cp_param.radio_hdr.start_symbol &&
        symbol < (cp_param.radio_hdr.start_symbol + cp_param.nof_symbols) &&
        filter_index == cp_param.radio_hdr.filter_index) {
      return cp_param;
    }

    return default_error_t{};
  }
};

} // namespace ofh
} // namespace srsran
