/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/pdcch/coreset.h"
#include <bitset>

namespace srsgnb {

/// \remark See TS 38.331, "SearchSpaceId" - The number of Search Spaces per BWP is limited to 10 including the common
/// and UE specific Search Spaces.
constexpr size_t MAX_NOF_SEARCH_SPACE_PER_BWP = 10;

/// \brief Search Space identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofSearchSpaces=40" Search Spaces configured. The ID space is used across BWPs of a serving cell.
enum search_space_id : uint8_t { MIN_SEARCH_SPACE_ID = 0, MAX_SEARCH_SPACE_ID = 39, MAX_NOF_SEARCH_SPACES = 40 };

inline search_space_id to_search_space_id(unsigned ss_id)
{
  return static_cast<search_space_id>(ss_id);
}
inline bool is_search_space_valid(search_space_id ss_id)
{
  return ss_id < MAX_NOF_SEARCH_SPACES;
}

/// Search Space Set Type as per TS38.213, Section 10.1.
enum class search_space_set_type { type0, type0A, type1, type2, type3, ue_specific };

/// Checks whether the SearchSpace set is common or dedicated.
inline bool is_common_search_space(search_space_set_type ss_set)
{
  return ss_set != search_space_set_type::ue_specific;
}

/// SearchSpace configuration as per TS38.331, "SearchSpace".
struct search_space_configuration {
  /// SearchSpace Type.
  enum class type_t { common, ue_dedicated };
  /// SearchSpace Common Type DCI Formats.
  struct common_dci_format {
    bool f0_0_and_f1_0;
    bool f2_0;
    bool f2_1;
    bool f2_2;
    bool f2_3;

    bool operator==(const common_dci_format& rhs) const
    {
      return f0_0_and_f1_0 == rhs.f0_0_and_f1_0 and f2_0 == rhs.f2_0 and f2_1 == rhs.f2_1 and f2_2 == rhs.f2_2 and
             f2_3 == rhs.f2_3;
    }
  };
  /// SearchSpace UE-specific DCI formats.
  enum class ue_specific_dci_format { f0_0_and_f1_0, f0_1_and_1_1 };

  search_space_id id;
  coreset_id      cs_id;
  /// Periodicity of slots for PDCCH monitoring. Possible values: {1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320, 640,
  /// 1280, 2560}.
  unsigned monitoring_slot_period;
  /// Offset of slot for PDCCH monitoring. Possible values: {0, ..., monitoring_slot_period}.
  unsigned monitoring_slot_offset;
  /// Number of consecutive slots that a SearchSpace lasts in every occasion. Values: (1..2559). If the field is absent,
  /// the UE applies the value 1 slot, except for DCI format 2_0. The UE ignores this field for DCI format 2_0.
  unsigned duration;
  /// The first symbol(s) for PDCCH monitoring in the slots for PDCCH monitoring. The most significant bit represents
  /// the first OFDM in a slot.
  optional<std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP>> monitoring_symbols_within_slot;
  /// Number of PDCCH candidates per aggregation level. The aggregation level for the array element with index "x"
  /// is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  std::array<uint8_t, 5> nof_candidates;
  type_t                 type;
  union {
    common_dci_format      common;
    ue_specific_dci_format ue_specific;
  };

  bool operator==(const search_space_configuration& rhs) const
  {
    return id == rhs.id and cs_id == rhs.cs_id and monitoring_slot_period == rhs.monitoring_slot_period and
           monitoring_slot_offset == rhs.monitoring_slot_offset and duration == rhs.duration and
           monitoring_symbols_within_slot == rhs.monitoring_symbols_within_slot and
           nof_candidates == rhs.nof_candidates and type == rhs.type and
           ((type == type_t::common and common == rhs.common) or
            (type == type_t::ue_dedicated and ue_specific == rhs.ue_specific));
  }
};

} // namespace srsgnb