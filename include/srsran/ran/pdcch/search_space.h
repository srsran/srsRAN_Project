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

#include "srsran/adt/variant.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/sched_consts.h"

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
  /// SearchSpace monitoring symbols within a slot.
  /// The first symbol(s) for PDCCH monitoring occasion(s) in the slots for PDCCH monitoring. The most
  /// significant bit represents the first OFDM in a slot.
  using monitoring_symbols_within_slot_t = bounded_bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, true>;
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

  /// Constructor for SearchSpace#0.
  explicit search_space_configuration(nr_band            band,
                                      subcarrier_spacing common_scs,
                                      subcarrier_spacing ssb_scs,
                                      unsigned           coreset0_index,
                                      unsigned           search_space0_index);

  /// Constructor for non-SearchSpace#0 SearchSpaces.
  explicit search_space_configuration(search_space_id                                    id_,
                                      coreset_id                                         cs_id_,
                                      std::array<uint8_t, 5>                             nof_candidates_,
                                      variant<common_dci_format, ue_specific_dci_format> dci_fmt_,
                                      unsigned                                           monitoring_slot_periodicity_,
                                      unsigned                                           monitoring_slot_offset_,
                                      subcarrier_spacing                                 scs_common,
                                      unsigned                                           duration_,
                                      monitoring_symbols_within_slot_t monitoring_symbols_within_slot_);

  bool operator==(const search_space_configuration& rhs) const
  {
    return id == rhs.id and cs_id == rhs.cs_id and monitoring_slot_periodicity == rhs.monitoring_slot_periodicity and
           monitoring_slot_offset == rhs.monitoring_slot_offset and duration == rhs.duration and
           monitoring_symbols_within_slot == rhs.monitoring_symbols_within_slot and
           nof_candidates == rhs.nof_candidates and dci_fmt == rhs.dci_fmt;
  }

  /// \brief Returns Id of the SearchSpace.
  search_space_id get_id() const { return id; }

  /// \brief Sets the Id for non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_id(search_space_id id_)
  {
    srsran_assert(not is_search_space0(), "Invalid access to SearchSpace Id of SearchSpace#0");
    srsran_assert(id_ != to_search_space_id(0), "Invalid access to SearchSpace Id of SearchSpace#0");
    id = id_;
  }

  /// \brief Returns CORESET Id of the SearchSpace.
  coreset_id get_coreset_id() const { return cs_id; }

  /// \brief Sets the CORESET Id for non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_coreset_id(coreset_id cs_id_)
  {
    srsran_assert(not is_search_space0(), "Invalid access to CORESET Id of SearchSpace#0");
    cs_id = cs_id_;
  }

  /// \brief Returns whether its a SearchSpace#0 or other SearchSpace.
  bool is_search_space0() const { return id == search_space_id(0); }

  /// \brief Returns whether SearchSpace if of Common SearchSpace(CSS) or UE Specific SearchSpace(USS).
  bool is_common_search_space() const { return variant_holds_alternative<common_dci_format>(dci_fmt); }

  /// \brief Sets the nof. PDCCH candidates for non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_nof_candidates(std::array<uint8_t, 5> nof_candidates_)
  {
    srsran_assert(not is_search_space0(), "Invalid access to number of PDCCH candidates of SearchSpace#0");
    nof_candidates = nof_candidates_;
  }

  /// \brief Returns the nof. PDCCH candidates monitoring in the SearchSpace.
  span<const uint8_t> get_nof_candidates() const { return nof_candidates; }

  /// \brief Sets the DCI format(s) monitored in non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_monitored_dci_formats(variant<common_dci_format, ue_specific_dci_format> dci_fmt_)
  {
    srsran_assert(not is_search_space0(), "Invalid access to DCI format(s) monitored of SearchSpace#0");
    dci_fmt = dci_fmt_;
  }

  /// \brief Returns DCI format(s) monitored in the SearchSpace.
  const variant<common_dci_format, ue_specific_dci_format>& get_monitored_dci_formats() const { return dci_fmt; }

  /// \brief Sets the periodicity in number of slots for non-SearchSpace#0 SearchSpaces
  void set_non_ss0_monitoring_slot_periodicity(unsigned periodicity)
  {
    srsran_assert(not is_search_space0(), "Invalid access to monitoring slot period of SearchSpace#0");
    monitoring_slot_periodicity = periodicity;
  }

  /// \brief Returns the SearchSpace periodicity in number of slots.
  unsigned get_monitoring_slot_periodicity() const { return monitoring_slot_periodicity; }

  /// \brief Sets the slot offset for non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_monitoring_slot_offset(unsigned slot_offset, subcarrier_spacing scs_common);

  /// \brief Returns the SearchSpace slot offset.
  unsigned get_monitoring_slot_offset(uint8_t ssb_beam_idx = 0) const
  {
    if (is_search_space0()) {
      srsran_assert(ssb_beam_idx < monitoring_slot_offset.size(), "Invalid SSB beam index={}", ssb_beam_idx);
      return monitoring_slot_offset[ssb_beam_idx].to_uint();
    }
    srsran_assert(ssb_beam_idx == 0, "SSB beam index is not used for SearchSpace Id > 0");
    return monitoring_slot_offset.back().to_uint();
  }

  /// \brief Sets the duration in number of slots for non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_duration(unsigned duration_)
  {
    srsran_assert(not is_search_space0(), "Invalid access to duration of SearchSpace#0");
    duration = duration_;
  }

  /// \brief Returns the duration that a SearchSpace lasts in every occasion, i.e. upon every period as given in the
  /// periodicity and offset, as per TS 38.331, "SearchSpace".
  unsigned get_duration() const { return duration; }

  /// \brief Sets the monitoring symbols within slot for non-SearchSpace#0 SearchSpaces.
  void set_non_ss0_monitoring_symbols_within_slot(monitoring_symbols_within_slot_t symbols_within_slot);

  /// \brief Returns the PDCCH monitoring symbols within slot.
  const monitoring_symbols_within_slot_t& get_monitoring_symbols_within_slot(uint8_t ssb_beam_idx = 0) const
  {
    if (is_search_space0()) {
      // TODO: Revise this when FR2 support is added.
      // As per TS 38.213, Table 13-11.
      srsran_assert(ssb_beam_idx < monitoring_symbols_within_slot.size(), "Invalid SSB beam index={}", ssb_beam_idx);
      return monitoring_symbols_within_slot[ssb_beam_idx];
    }
    srsran_assert(ssb_beam_idx == 0, "SSB beam index is not used for SearchSpace Id > 0");
    return monitoring_symbols_within_slot.back();
  }

  /// \brief Returns the first monitoring symbol for the current SearchSpace.
  unsigned get_first_symbol_index(uint8_t ssb_beam_idx = 0) const
  {
    monitoring_symbols_within_slot_t symbols_within_slot;
    if (is_search_space0()) {
      // TODO: Revise this when FR2 support is added.
      // As per TS 38.213, Table 13-11.
      srsran_assert(ssb_beam_idx < monitoring_symbols_within_slot.size(), "Invalid SSB beam index={}", ssb_beam_idx);
      symbols_within_slot = monitoring_symbols_within_slot[ssb_beam_idx];
    } else {
      srsran_assert(ssb_beam_idx == 0, "SSB beam index is not used for SearchSpace Id > 0");
      symbols_within_slot = monitoring_symbols_within_slot.back();
    }
    for (unsigned n = 0; n < symbols_within_slot.size(); ++n) {
      if (symbols_within_slot.test(n)) {
        return n;
      }
    }
    srsran_assertion_failure("Monitoring symbols within slot for SS id {} doesn't have any symbols set to 1", id);
    return symbols_within_slot.size();
  }

private:
  /// SearchSpace Id.
  search_space_id id;
  /// CORESET Id.
  coreset_id cs_id;
  /// Number of PDCCH candidates per aggregation level. The aggregation level for the array element with index "x"
  /// is L=1U << x. The possible values for each element are {0, 1, 2, 3, 4, 5, 6, 8}.
  std::array<uint8_t, 5> nof_candidates;
  /// DCI formats applicable to SearchSpace.
  variant<common_dci_format, ue_specific_dci_format> dci_fmt;
  /// SearchSpace periodicity in nof. slots for PDCCH monitoring.
  /// For SearchSpace == 0, set based on tables in TS 38.213, Section 13.
  /// For SearchSpace != 0, possible values: {1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320, 640, 1280, 2560}.
  unsigned monitoring_slot_periodicity;
  /// Number of consecutive slots that SearchSpace lasts in every occasion.
  /// For SearchSpace == 0, its 2 consecutive slots for multiplexing pattern 1.
  /// For SearchSpace != 0, Values: (1..2559). If the field is absent, the UE applies the value 1 slot, except for DCI
  /// format 2_0. The UE ignores this field for DCI format 2_0.
  unsigned duration;
  /// SearchSpace slot offset for PDCCH monitoring.
  /// For SearchSpace == 0, set based on tables in TS 38.213, Section 13. Each element in vector corresponds to a SSB
  /// beam of index equal to index in vector.
  /// For SearchSpace != 0, only the first element of the vector is used. Possible values:
  /// {0,...,monitoring_slot_period}.
  static_vector<slot_point, MAX_NUM_BEAMS> monitoring_slot_offset;
  /// The first symbol(s) for PDCCH monitoring occasion(s) in the slots for PDCCH monitoring. The most
  /// significant bit represents the first OFDM in a slot.
  /// For SearchSpace == 0, each element in vector corresponds to a SSB beam of index equal to index in vector.
  /// For SearchSpace != 0, only the first element of the vector is used.
  static_vector<monitoring_symbols_within_slot_t, MAX_NUM_BEAMS> monitoring_symbols_within_slot;
};

} // namespace srsran
