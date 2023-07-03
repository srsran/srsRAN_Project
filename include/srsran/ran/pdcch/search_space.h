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

#include "pdcch_type0_css_coreset_config.h"
#include "pdcch_type0_css_occasions.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/scheduler/sched_consts.h"
#include <bitset>
#include <cstdint>
#include <vector>

namespace srsran {

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

/// SearchSpace Type.
enum class search_space_type { common, ue_dedicated };

/// SearchSpace configuration as per TS38.331, "SearchSpace".
struct search_space_configuration {
  /// SearchSpace Type.
  using type_t = search_space_type;
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
    return id == rhs.id and cs_id == rhs.cs_id and monitoring_slot_periodicity == rhs.monitoring_slot_periodicity and
           monitoring_slot_offset == rhs.monitoring_slot_offset and duration == rhs.duration and
           ss_params == rhs.ss_params and nof_candidates == rhs.nof_candidates and type == rhs.type and
           ((type == type_t::common and common == rhs.common) or
            (type == type_t::ue_dedicated and ue_specific == rhs.ue_specific));
  }

  /// \brief Returns the first monitoring symbol for the current SearchSpace.
  unsigned get_first_symbol_index(uint8_t ssb_beam_idx = 0) const
  {
    std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP> monitoring_symbols_within_slot;
    if (id == search_space_id(0)) {
      // TODO: Revise this when FR2 support is added.
      srsran_assert(variant_holds_alternative<ss0_params>(ss_params),
                    "Monitoring symbols within slot not set for SearchSpace#0");
      const auto& cfg_monitoring_symb_in_slot_per_ssb_beam =
          variant_get<ss0_params>(ss_params).ss0_monitoring_symbols_within_slot_per_ssb_beam;
      // As per TS 38.213, Table 13-11.
      srsran_assert(
          ssb_beam_idx < cfg_monitoring_symb_in_slot_per_ssb_beam.size(), "Invalid SSB beam index={}", ssb_beam_idx);
      monitoring_symbols_within_slot = cfg_monitoring_symb_in_slot_per_ssb_beam[ssb_beam_idx];
    } else {
      srsran_assert(variant_holds_alternative<other_ss_params>(ss_params),
                    "Monitoring symbols within slot not set for SearchSpace Id > 0");
      const auto& cfg_monitoring_symb_in_slot_per_ssb_beam =
          variant_get<other_ss_params>(ss_params).other_ss_monitoring_symbols_within_slot;
      monitoring_symbols_within_slot = cfg_monitoring_symb_in_slot_per_ssb_beam;
    }
    for (unsigned n = 0; n < monitoring_symbols_within_slot.size(); ++n) {
      if (monitoring_symbols_within_slot.test(monitoring_symbols_within_slot.size() - n - 1)) {
        return n;
      }
    }
    srsran_assertion_failure("Monitoring symbols within slot for SS id {} doesn't have any symbols set to 1", id);
    return monitoring_symbols_within_slot.size();
  }

  /// \brief Sets the SearchSpace#0 periodicity in number of slots.
  void set_ss0_monitoring_slot_periodicity(const cell_config_builder_params& params)
  {
    srsran_assert(id == search_space_id(0),
                  "Invalid access to SearchSpace#0 monitoring slot period for a SearchSpace Id > 0");
    // NOTE: Currently, we support only SS/PBCH and CORESET multiplexing pattern 1 where the periodicity of
    // SearchSpace#0 is 20 ms.
    // TODO: Revise the below assignment when other multiplexing patterns are supported.
    monitoring_slot_periodicity = get_nof_slots_per_subframe(params.scs_common) * 20;
  }

  /// \brief Sets the non-SearchSpace#0 periodicity in number of slots.
  void set_non_ss0_monitoring_slot_periodicity(unsigned periodicity)
  {
    srsran_assert(
        id != search_space_id(0), "Invalid access to SearchSpace#{} monitoring slot period for a SearchSpace#0", id);
    monitoring_slot_periodicity = periodicity;
  }

  /// \brief Returns the SearchSpace periodicity in number of slots.
  unsigned get_monitoring_slot_periodicity() const { return monitoring_slot_periodicity; }

  /// \brief Sets the SearchSpace#0 slot offset.
  void set_ss0_monitoring_slot_offset(const cell_config_builder_params& params)
  {
    srsran_assert(id == search_space_id(0),
                  "Invalid access to SearchSpace#0 monitoring slot offset for a SearchSpace Id > 0");
    const pdcch_type0_css_occasion_pattern1_description ss0_occasion = fetch_ss0_occasion_info(params);
    for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ++ssb_idx) {
      monitoring_slot_offset.push_back(srsran::get_type0_pdcch_css_n0(
          static_cast<unsigned>(ss0_occasion.offset), ss0_occasion.M, params.scs_common, ssb_idx));
    }
  }

  /// \brief Sets the non-SearchSpace#0 slot offset.
  void set_non_ss0_monitoring_slot_offset(unsigned slot_offset, subcarrier_spacing scs_common)
  {
    srsran_assert(
        id != search_space_id(0), "Invalid access to SearchSpace#{} monitoring slot offset for a SearchSpace#0", id);
    const uint8_t numerology_mu = to_numerology_value(scs_common);
    monitoring_slot_offset.push_back({numerology_mu, slot_offset});
  }

  /// \brief Returns the SearchSpace slot offset.
  unsigned get_monitoring_slot_offset(uint8_t ssb_beam_idx = 0) const
  {
    if (id == search_space_id(0)) {
      srsran_assert(ssb_beam_idx < monitoring_slot_offset.size(), "Invalid SSB beam index={}", ssb_beam_idx);
      return monitoring_slot_offset[ssb_beam_idx].to_uint();
    }
    return monitoring_slot_offset.back().to_uint();
  }

  /// \brief Sets the SearchSpace#0 duration in number of slots.
  void set_ss0_duration()
  {
    srsran_assert(id == search_space_id(0), "Invalid access to SearchSpace#0 duration for a SearchSpace Id > 0");
    // NOTE: Currently, we support only SS/PBCH and CORESET multiplexing pattern 1 where the duration of SearchSpace#0
    // is 2 consecutive slots.
    // TODO: Revise the below assignment when other multiplexing patterns are supported.
    duration = 2;
  }

  /// \brief Sets the non-SearchSpace#0 duration in number of slots.
  void set_non_ss0_duration(unsigned duration_)
  {
    srsran_assert(id != search_space_id(0), "Invalid access to SearchSpace#{} duration for a SearchSpace#0", id);
    duration = duration_;
  }

  /// \brief Returns the SearchSpace duration in number of slots.
  unsigned get_duration() const { return duration; }

  void set_ss0_monitoring_symbols_within_slot(const cell_config_builder_params& params)
  {
    srsran_assert(id == search_space_id(0),
                  "Invalid access to SearchSpace#0 monitoring symbols within slot for a SearchSpace Id > 0");
    ss0_params                                          monitoring_params{};
    const pdcch_type0_css_occasion_pattern1_description ss0_occasion = fetch_ss0_occasion_info(params);
    for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ++ssb_idx) {
      monitoring_params.ss0_monitoring_symbols_within_slot_per_ssb_beam.emplace_back();
      monitoring_params.ss0_monitoring_symbols_within_slot_per_ssb_beam.back().set(
          monitoring_params.ss0_monitoring_symbols_within_slot_per_ssb_beam.back().size() - 1 -
              ss0_occasion.start_symbol[ssb_idx % ss0_occasion.start_symbol.size()],
          true);
    }
    ss_params = monitoring_params;
  }

  void set_non_ss0_monitoring_symbols_within_slot(
      std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP> monitoring_symbols_within_slot)
  {
    srsran_assert(id != search_space_id(0),
                  "Invalid access to SearchSpace#{} monitoring symbols within slot for a SearchSpace#0",
                  id);
    srsran_assert(not monitoring_symbols_within_slot.none(),
                  "None of the symbols are set in monitoring symbols within a slot when configuring SearchSpace#{}",
                  id);
    other_ss_params monitoring_params{};
    monitoring_params.other_ss_monitoring_symbols_within_slot = monitoring_symbols_within_slot;
    ss_params                                                 = monitoring_params;
  }

  std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP> get_monitoring_symbols_within_slot(uint8_t ssb_beam_idx = 0) const
  {
    if (id == search_space_id(0)) {
      // TODO: Revise this when FR2 support is added.
      srsran_assert(variant_holds_alternative<ss0_params>(ss_params),
                    "Monitoring symbols within slot not set for SearchSpace#0");
      const auto& cfg_monitoring_symb_in_slot_per_ssb_beam =
          variant_get<ss0_params>(ss_params).ss0_monitoring_symbols_within_slot_per_ssb_beam;
      // As per TS 38.213, Table 13-11.
      srsran_assert(
          ssb_beam_idx < cfg_monitoring_symb_in_slot_per_ssb_beam.size(), "Invalid SSB beam index={}", ssb_beam_idx);
      return cfg_monitoring_symb_in_slot_per_ssb_beam[ssb_beam_idx];
    }
    srsran_assert(variant_holds_alternative<other_ss_params>(ss_params),
                  "Monitoring symbols within slot not set for SearchSpace Id > 0");
    return variant_get<other_ss_params>(ss_params).other_ss_monitoring_symbols_within_slot;
  }

private:
  /// Parameters specific to SearchSpace#0.
  struct ss0_params {
    /// The first symbol(s) for PDCCH monitoring in the slots for PDCCH monitoring per SSB beam. The most significant
    /// bit represents the first OFDM in a slot. Each element in vector corresponds to a SSB beam of index equal to
    /// index in vector.
    static_vector<std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP>, MAX_NUM_BEAMS>
        ss0_monitoring_symbols_within_slot_per_ssb_beam;

    bool operator==(const ss0_params& rhs) const
    {
      return ss0_monitoring_symbols_within_slot_per_ssb_beam == rhs.ss0_monitoring_symbols_within_slot_per_ssb_beam;
    }
    bool operator!=(const ss0_params& rhs) const { return !(rhs == *this); }
  };

  /// Parameters specific to SearchSpaces other than SearchSpace#0.
  struct other_ss_params {
    /// The first symbol(s) for PDCCH monitoring in the slots for PDCCH monitoring for SearchSpaces other than
    /// SearchSpace#0. The most significant bit represents the first OFDM in a slot.
    std::bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP> other_ss_monitoring_symbols_within_slot;

    bool operator==(const other_ss_params& rhs) const
    {
      return other_ss_monitoring_symbols_within_slot == rhs.other_ss_monitoring_symbols_within_slot;
    }
    bool operator!=(const other_ss_params& rhs) const { return !(rhs == *this); }
  };

  /// \brief Helper function to fetch SearchSpace#0 monitoring occasion information.
  static pdcch_type0_css_occasion_pattern1_description fetch_ss0_occasion_info(const cell_config_builder_params& params)
  {
    const auto band = params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);

    const pdcch_type0_css_coreset_description desc =
        pdcch_type0_css_coreset_get(band, params.scs_common, params.scs_common, params.coreset0_index, 0);
    return pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
        .is_fr2           = false,
        .ss_zero_index    = static_cast<uint8_t>(params.search_space0_index),
        .nof_symb_coreset = desc.nof_symb_coreset});
  }

  /// SearchSpace periodicity in nof. slots for PDCCH monitoring.
  /// For SearchSpace == 0, set based on tables in TS 38.213, Section 13.
  /// For SearchSpace != 0, possible values: {1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320, 640, 1280, 2560}.
  unsigned monitoring_slot_periodicity;
  /// Number of consecutive slots that SearchSpace lasts in every occasion.
  /// For SearchSpace != 0, Values: (1..2559). If the field is absent, the UE applies the value 1 slot, except for DCI
  /// format 2_0. The UE ignores this field for DCI format 2_0.
  unsigned duration;
  /// SearchSpace slot offset for PDCCH monitoring.
  /// For SearchSpace == 0, set based on tables in TS 38.213, Section 13. Each element in vector corresponds to a SSB
  /// beam of index equal to index in vector.
  /// For SearchSpace != 0, possible values: {0,...,monitoring_slot_period}.
  static_vector<slot_point, MAX_NUM_BEAMS> monitoring_slot_offset;
  variant<ss0_params, other_ss_params>     ss_params;
};

} // namespace srsran
