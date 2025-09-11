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

/// \file Defines the parameters that define the PUCCH struct and enums. These parameters are derived from the related
/// ones given in TS 38.331, \c PUCCH-Config.

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/sr_configuration.h"
#include <cstdint>
#include <variant>

namespace srsran {

/// Maximum number of PUCCH Resource Sets.
/// \remark See TS 38.331, "maxNrofPUCCH-ResourceSets".
constexpr size_t MAX_NOF_PUCCH_RESOURCE_SETS = 4;

/// Maximum number of PUCCH resources that can be configured for a UE.
/// \remark See TS 38.331, "maxNrofPUCCH-Resources".
constexpr size_t MAX_NOF_UE_PUCCH_RESOURCES = 128;

/// Maximum number of PUCCH Resources per PUCCH-ResourceSet.
/// \remark See TS 38.331, "maxNrofPUCCH-ResourcesPerSet". Only valid for the first \c pucch_resource_set, see
/// TS 38.213, Section 9.2.1.
constexpr size_t MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET = 32;

/// Maximum number of PUCCH p0 per set, as per "maxNrofPUCCH-P0-PerSet", TS 38.331.
constexpr size_t MAX_NOF_PUCCH_P0_PER_SET = 8;

/// Maximum number of PUCCH path-path loss references RSs per set, as per "maxNrofPUCCH-PathlossReferenceRSs",
/// TS 38.331.
constexpr size_t MAX_NOF_PUCCH_PATHLOSS_REF_RS_PER_SET = 4;

/// Options for \c occ-Length in \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
enum class pucch_f4_occ_len { n2 = 2, n4 = 4 };

/// Options for \c occ-Index in \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
enum class pucch_f4_occ_idx { n0, n1, n2, n3 };

/// Options for \c PUCCH-MaxCodeRate in \c PUCCH-Config, TS 38.331.
enum class max_pucch_code_rate { not_set = 0, dot_08, dot_15, dot_25, dot_35, dot_45, dot_60, dot_80 };

inline float to_max_code_rate_float(max_pucch_code_rate opt)
{
  switch (opt) {
    case max_pucch_code_rate::dot_08:
      return 0.08f;
    case max_pucch_code_rate::dot_15:
      return 0.15f;
    case max_pucch_code_rate::dot_25:
      return 0.25f;
    case max_pucch_code_rate::dot_35:
      return 0.35f;
    case max_pucch_code_rate::dot_45:
      return 0.45f;
    case max_pucch_code_rate::dot_60:
      return 0.60f;
    case max_pucch_code_rate::dot_80:
      return 0.80f;
    default:
      break;
  }
  return 0;
}

/// \c PUCCH-FormatConfig, in \c PUCCH-Config, TS 38.331. It describes the parameters that are common for a given PUCCH
/// format.
struct pucch_common_all_formats {
  enum class num_of_slots { not_set = 0, n2, n4, n8 };

  // The false or not_set value indicates that the field is optional and not set.
  bool                interslot_freq_hop{false};
  bool                additional_dmrs{false};
  max_pucch_code_rate max_c_rate{max_pucch_code_rate::not_set};
  num_of_slots        nof_slots{num_of_slots::not_set};
  bool                pi_2_bpsk{false};
  bool                simultaneous_harq_ack_csi{false};

  bool operator==(const pucch_common_all_formats& rhs) const
  {
    return interslot_freq_hop == rhs.interslot_freq_hop && additional_dmrs == rhs.additional_dmrs &&
           max_c_rate == rhs.max_c_rate && nof_slots == rhs.nof_slots && pi_2_bpsk == rhs.pi_2_bpsk &&
           simultaneous_harq_ack_csi == rhs.simultaneous_harq_ack_csi;
  }
  bool operator!=(const pucch_common_all_formats& rhs) const { return !(rhs == *this); }
};

/// Configuration for \c PUCCH-format0, in \c PUCCH-Config, TS 38.331.
struct pucch_format_0_cfg {
  uint8_t initial_cyclic_shift;

  bool operator==(const pucch_format_0_cfg& rhs) const { return initial_cyclic_shift == rhs.initial_cyclic_shift; }
  bool operator!=(const pucch_format_0_cfg& rhs) const { return !(rhs == *this); }
};

/// Configuration for \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
struct pucch_format_1_cfg {
  uint8_t initial_cyclic_shift;
  uint8_t time_domain_occ;

  bool operator==(const pucch_format_1_cfg& rhs) const
  {
    return initial_cyclic_shift == rhs.initial_cyclic_shift && time_domain_occ == rhs.time_domain_occ;
  }
  bool operator!=(const pucch_format_1_cfg& rhs) const { return !(rhs == *this); }
};

/// Configuration for \c PUCCH-format2 or \c PUCCH-format3, in \c PUCCH-Config, TS 38.331.
struct pucch_format_2_3_cfg {
  uint8_t nof_prbs;

  bool operator==(const pucch_format_2_3_cfg& rhs) const { return nof_prbs == rhs.nof_prbs; }
  bool operator!=(const pucch_format_2_3_cfg& rhs) const { return !(rhs == *this); }
};

/// Configuration for \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
struct pucch_format_4_cfg {
  pucch_f4_occ_len occ_length;
  pucch_f4_occ_idx occ_index;

  bool operator==(const pucch_format_4_cfg& rhs) const
  {
    return occ_length == rhs.occ_length && occ_index == rhs.occ_index;
  }
  bool operator!=(const pucch_format_4_cfg& rhs) const { return !(rhs == *this); }
};

/// \c PUCCH-Resource, in \c PUCCH-Config, TS 38.331.
struct pucch_resource {
  pucch_res_id_t                                                                                 res_id = {0, 0};
  unsigned                                                                                       starting_prb;
  std::optional<unsigned>                                                                        second_hop_prb;
  uint8_t                                                                                        nof_symbols;
  uint8_t                                                                                        starting_sym_idx;
  pucch_format                                                                                   format;
  std::variant<pucch_format_0_cfg, pucch_format_1_cfg, pucch_format_2_3_cfg, pucch_format_4_cfg> format_params;

  bool operator==(const pucch_resource& rhs) const
  {
    return res_id == rhs.res_id && starting_prb == rhs.starting_prb && second_hop_prb == rhs.second_hop_prb &&
           nof_symbols == rhs.nof_symbols && starting_sym_idx == rhs.starting_sym_idx && format == rhs.format &&
           format_params == rhs.format_params;
  }
  bool operator!=(const pucch_resource& rhs) const { return !(rhs == *this); }
};

/// \c PUCCH-ResourceSet, in \c PUCCH-Config, TS 38.331.
/// \remark The field \c pucch-ResourceSetId is not included, as we reference the resource set ID explicitly in
/// \ref pucch_config.
struct pucch_resource_set {
  /// \c PUCCH-ResourceSetId.
  pucch_res_set_idx pucch_res_set_id;
  /// \c resourceList.
  static_vector<pucch_res_id_t, MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET> pucch_res_id_list;
  /// \c maxPayloadSize.
  std::optional<unsigned> max_payload_size;

  bool operator==(const pucch_resource_set& rhs) const
  {
    return pucch_res_set_id == rhs.pucch_res_set_id && pucch_res_id_list == rhs.pucch_res_id_list &&
           max_payload_size == rhs.max_payload_size;
  }
  bool operator!=(const pucch_resource_set& rhs) const { return !(rhs == *this); }
};

/// PUCCH power control configuration, as "PUCCH-PowerControl", TS 38.331.
struct pucch_power_control {
  struct p0_pucch {
    uint8_t                       id;
    bounded_integer<int, -16, 15> value;

    bool operator==(const p0_pucch& rhs) const { return id == rhs.id && value == rhs.value; }
    bool operator!=(const p0_pucch& rhs) const { return !(rhs == *this); }
  };

  ///  PUCCH - PathlossReferenceRS
  struct pl_reference_rs {
    uint8_t id;
    /// If true, the reference signal is a SSB beam, otherwise it is a CSI-RS.
    bool use_ssb_idx;
    /// Index of the SSB beam or CSI-RS to be used as reference signal for path-loss estimation.
    /// Values {0..63} for SSB beam, {0..191} for CSI-RS.
    uint8_t reference_signal_id;

    bool operator==(const pl_reference_rs& rhs) const
    {
      return id == rhs.id && use_ssb_idx == rhs.use_ssb_idx && reference_signal_id == rhs.reference_signal_id;
    }
    bool operator!=(const pl_reference_rs& rhs) const { return !(rhs == *this); }
  };

  std::optional<bounded_integer<int, -16, 15>> delta_pucch_f0;
  std::optional<bounded_integer<int, -16, 15>> delta_pucch_f1;
  std::optional<bounded_integer<int, -16, 15>> delta_pucch_f2;
  std::optional<bounded_integer<int, -16, 15>> delta_pucch_f3;
  std::optional<bounded_integer<int, -16, 15>> delta_pucch_f4;

  static_vector<p0_pucch, MAX_NOF_PUCCH_P0_PER_SET>                     p0_set;
  static_vector<pl_reference_rs, MAX_NOF_PUCCH_PATHLOSS_REF_RS_PER_SET> pl_reference_rs_set;
  bool                                                                  two_states_adjustment = false;

  bool operator==(const pucch_power_control& rhs) const
  {
    return delta_pucch_f0 == rhs.delta_pucch_f0 && delta_pucch_f1 == rhs.delta_pucch_f1 &&
           delta_pucch_f2 == rhs.delta_pucch_f2 && delta_pucch_f3 == rhs.delta_pucch_f3 &&
           delta_pucch_f4 == rhs.delta_pucch_f4 && p0_set == rhs.p0_set &&
           pl_reference_rs_set == rhs.pl_reference_rs_set && two_states_adjustment == rhs.two_states_adjustment;
  }

  bool operator!=(const pucch_power_control& rhs) const { return !(rhs == *this); }
};

/// \c PUCCH-Config, TS 38.331.
struct pucch_config {
  /// \c PUCCH-ResourceSet, from 0 to 3.
  // NOTE: PUCCH resource set ID 0 can only contain PUCCH format 0 and 1.
  static_vector<pucch_resource_set, MAX_NOF_PUCCH_RESOURCE_SETS> pucch_res_set;
  /// List of \c PUCCH-Resource.
  static_vector<pucch_resource, MAX_NOF_UE_PUCCH_RESOURCES> pucch_res_list;
  /// \c format1 .. \c format4, which contain the parameters that are common to a given PUCCH Format.
  std::optional<pucch_common_all_formats> format_1_common_param;
  std::optional<pucch_common_all_formats> format_2_common_param;
  std::optional<pucch_common_all_formats> format_3_common_param;
  std::optional<pucch_common_all_formats> format_4_common_param;

  /// List of \c SchedulingRequestResourceConfig.
  static_vector<scheduling_request_resource_config, MAX_NOF_SR_RESOURCES> sr_res_list;

  /// \c dl-DataToUL-ACK. Values {0..15}.
  static_vector<uint8_t, 8> dl_data_to_ul_ack;

  /// PUCCH resource max UCI payload, depending on the format. The index defines the format.
  /// \remark The UCI payload is the same for all UE's PUCCH resources belonging to the same format, regardless of
  /// whether they are used for HARQ-ACK or CSI.
  /// \remark For Format 0 and 1, only the max number of HARQ-ACK bits are considered.
  static_vector<unsigned, 5> format_max_payload{0, 0, 0, 0, 0};

  /// Helper variable to avoid iterating over several lists to retrieve the PUCCH format.
  pucch_format set_1_format = pucch_format::NOF_FORMATS;

  std::optional<pucch_power_control> pucch_pw_control;

  /// Returns the PUCCH resource max UCI payload for the given format.
  /// \remark For Format 0 and 1, it returns only the max number of HARQ-ACK bits.
  unsigned get_max_payload(pucch_format format) const { return format_max_payload[pucch_format_to_uint(format)]; }

  /// Returns the PUCCH format of the resources from PUCCH resource set 1.
  /// \remark The PUCCH resource set 1 can only contain PUCCH format 2, 3 or 4.
  pucch_format get_set_1_format() const { return set_1_format; }

  bool operator==(const pucch_config& rhs) const
  {
    return pucch_res_set == rhs.pucch_res_set && pucch_res_list == rhs.pucch_res_list &&
           format_1_common_param == rhs.format_1_common_param && format_2_common_param == rhs.format_2_common_param &&
           format_3_common_param == rhs.format_3_common_param && format_4_common_param == rhs.format_4_common_param &&
           sr_res_list == rhs.sr_res_list && dl_data_to_ul_ack == rhs.dl_data_to_ul_ack &&
           pucch_pw_control == rhs.pucch_pw_control;
  }
  bool operator!=(const pucch_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
