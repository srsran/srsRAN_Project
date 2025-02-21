/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "context_repository_helpers.h"
#include "srsran/adt/expected.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_cplane_message_properties.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
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
  using repo_entry = std::array<ul_cplane_context, MAX_SUPPORTED_EAXC_ID_VALUE>;

  std::vector<repo_entry> repo;
  //: TODO: make this lock free
  mutable std::mutex mutex;
  /// Ignore the start symbol value received in the PRACH U-Plane packets.
  bool ignore_prach_start_symbol;

  /// Returns the entry of the repository for the given slot and eAxC.
  ul_cplane_context& entry(slot_point slot, unsigned eaxc)
  {
    unsigned index = calculate_repository_index(slot, repo.size());
    return repo[index][eaxc];
  }

  /// Returns the entry of the repository for the given slot and eAxC.
  const ul_cplane_context& entry(slot_point slot, unsigned eaxc) const
  {
    unsigned index = calculate_repository_index(slot, repo.size());
    return repo[index][eaxc];
  }

public:
  explicit uplink_cplane_context_repository(unsigned size_, bool ignore_prach_start_symbol_ = false) :
    repo(size_), ignore_prach_start_symbol(ignore_prach_start_symbol_)
  {
    static_assert(MAX_PRACH_OCCASIONS_PER_SLOT == 1,
                  "Uplink Control-Plane context repository only supports one context per slot and eAxC");
  }

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

    auto is_start_symbol_valid = [this, &cp_param](unsigned start_symbol) {
      if (ignore_prach_start_symbol &&
          (cp_param.radio_hdr.filter_index == filter_index_type::ul_prach_preamble_1p25khz)) {
        // Some RUs always set PRACH symbolId to 0 when long format is used ignoring the value indicated in C-Plane.
        if (start_symbol >= cp_param.radio_hdr.start_symbol) {
          start_symbol -= cp_param.radio_hdr.start_symbol;
        }
        return start_symbol < cp_param.nof_symbols;
      }

      return (start_symbol >= cp_param.radio_hdr.start_symbol) &&
             (start_symbol < (cp_param.radio_hdr.start_symbol + cp_param.nof_symbols));
    };

    if (filter_index == cp_param.radio_hdr.filter_index && is_start_symbol_valid(symbol)) {
      return cp_param;
    }

    return make_unexpected(default_error_t{});
  }
};

} // namespace ofh
} // namespace srsran
