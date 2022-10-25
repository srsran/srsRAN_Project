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

#include "pucch_mapping.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/sr_configuration.h"
#include <cstdint>
#include <vector>

namespace srsgnb {

/// Maximum number of PUCCH Resource Sets.
/// \remark See TS 38.331, "maxNrofPUCCH-ResourceSets".
constexpr size_t MAX_NOF_PUCCH_RESOURCE_SETS = 4;

/// Maximum number of PUCCH resources.
/// \remark See TS 38.331, "maxNrofPUCCH-Resources".
constexpr size_t MAX_NOF_PUCCH_RESOURCES = 128;

/// Maximum number of PUCCH Resources per PUCCH-ResourceSet.
/// \remark See TS 38.331, "maxNrofPUCCH-ResourcesPerSet".
constexpr size_t MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET = 32;

/// Options for \c occ-Length in \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
enum class pucch_f4_occ_len : uint8_t { n2 = 0, n4 };

/// Options for \c occ-Index in \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
enum class pucch_f4_occ_idx : uint8_t { n0 = 0, n1, n2, n3 };

/// \c PUCCH-FormatConfig, in \c PUCCH-Config, TS 38.331. It describes the parameters that are common for a given PUCCH
/// format.
struct pucch_common_all_formats {
  enum class max_code_rate : uint8_t { dot_08 = 0, dot_15, dot_25, dot_35, dot_45, dot_60, dot_80 };
  enum class num_of_slots : uint8_t { n2 = 0, n4, n8 };

  bool operator==(const pucch_common_all_formats& rhs) const
  {
    return interslot_freq_hop == rhs.interslot_freq_hop && additional_dmrs == rhs.additional_dmrs &&
           max_c_rate == rhs.max_c_rate && nof_slots == rhs.nof_slots && pi_2_bpsk == rhs.pi_2_bpsk &&
           simultaneous_harq_ack_csi == rhs.simultaneous_harq_ack_csi;
  }
  bool operator!=(const pucch_common_all_formats& rhs) const { return !(rhs == *this); }

  bool          interslot_freq_hop;
  bool          additional_dmrs;
  max_code_rate max_c_rate;
  num_of_slots  nof_slots;
  bool          pi_2_bpsk;
  bool          simultaneous_harq_ack_csi;
};

/// Configuration for \c PUCCH-format0, in \c PUCCH-Config, TS 38.331.
struct pucch_format_0_cfg {
  bool operator==(const pucch_format_0_cfg& rhs) const
  {
    return initial_cyclic_shift == rhs.initial_cyclic_shift && nof_symbols == rhs.nof_symbols &&
           starting_sym_idx == rhs.starting_sym_idx;
  }
  bool operator!=(const pucch_format_0_cfg& rhs) const { return !(rhs == *this); }

  uint8_t initial_cyclic_shift;
  uint8_t nof_symbols;
  uint8_t starting_sym_idx;
};

/// Configuration for \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
struct pucch_format_1_cfg {
  bool operator==(const pucch_format_1_cfg& rhs) const
  {
    return initial_cyclic_shift == rhs.initial_cyclic_shift && nof_symbols == rhs.nof_symbols &&
           starting_sym_idx == rhs.starting_sym_idx && time_domain_occ == rhs.time_domain_occ;
  }
  bool operator!=(const pucch_format_1_cfg& rhs) const { return !(rhs == *this); }

  uint8_t initial_cyclic_shift;
  uint8_t nof_symbols;
  uint8_t starting_sym_idx;
  uint8_t time_domain_occ;
};

/// Configuration for \c PUCCH-format2, in \c PUCCH-Config, TS 38.331.
struct pucch_format_2_3_cfg {
  bool operator==(const pucch_format_2_3_cfg& rhs) const
  {
    return nof_prbs == rhs.nof_prbs && nof_symbols == rhs.nof_symbols && starting_sym_idx == rhs.starting_sym_idx;
  }
  bool operator!=(const pucch_format_2_3_cfg& rhs) const { return !(rhs == *this); }

  uint8_t nof_prbs;
  uint8_t nof_symbols;
  uint8_t starting_sym_idx;
};

/// Configuration for \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
struct pucch_format_4_cfg {
  bool operator==(const pucch_format_4_cfg& rhs) const
  {
    return nof_symbols == rhs.nof_symbols && occ_length == rhs.occ_length && occ_index == rhs.occ_index &&
           starting_sym_idx == rhs.starting_sym_idx;
  }
  bool operator!=(const pucch_format_4_cfg& rhs) const { return !(rhs == *this); }

  uint8_t          nof_symbols;
  pucch_f4_occ_len occ_length;
  pucch_f4_occ_idx occ_index;
  uint8_t          starting_sym_idx;
};

/// \c PUCCH-Resource, in \c PUCCH-Config, TS 38.331.
struct pucch_resource {
  bool operator==(const pucch_resource& rhs) const
  {
    return res_id == rhs.res_id && starting_prb == rhs.starting_prb && second_hop_prb == rhs.second_hop_prb &&
           intraslot_freq_hopping == rhs.intraslot_freq_hopping && format == rhs.format && format_0 == rhs.format_0 &&
           format_1 == rhs.format_1 && format_2 == rhs.format_2 && format_3 == rhs.format_3 && format_4 == rhs.format_4;
  }
  bool operator!=(const pucch_resource& rhs) const { return !(rhs == *this); }

  unsigned     res_id;
  unsigned     starting_prb;
  unsigned     second_hop_prb;
  bool         intraslot_freq_hopping;
  pucch_format format;
  union {
    pucch_format_0_cfg   format_0;
    pucch_format_1_cfg   format_1;
    pucch_format_2_3_cfg format_2;
    pucch_format_2_3_cfg format_3;
    pucch_format_4_cfg   format_4;
  };
};

/// \c PUCCH-ResourceSet, in \c PUCCH-Config, TS 38.331.
/// \remark The field \c pucch-ResourceSetId is not included, as we reference the resource set ID explicitly in
/// \ref pucch_config.
struct pucch_resource_set {
  bool operator==(const pucch_resource_set& rhs) const
  {
    return pucch_res_set_id == rhs.pucch_res_set_id && pucch_res_id_list == rhs.pucch_res_id_list &&
           max_payload_size == rhs.max_payload_size;
  }
  bool operator!=(const pucch_resource_set& rhs) const { return !(rhs == *this); }

  /// \c PUCCH-ResourceSetId.
  uint8_t pucch_res_set_id;
  /// \c resourceList.
  static_vector<unsigned, MAX_NOF_PUCCH_RESOURCES_PER_PUCCH_RESOURCE_SET> pucch_res_id_list;
  /// \c maxPayloadSize.
  optional<unsigned> max_payload_size;
};

/// \c PUCCH-Config, TS 38.311.
struct pucch_config {
  /// \c PUCCH-ResourceSet, from 0 to 3.
  // NOTE: PUCCH resource set ID 0 can only contain PUCCH format 0 and 1.
  static_vector<pucch_resource_set, MAX_NOF_PUCCH_RESOURCE_SETS> pucch_res_set;
  /// List of \c PUCCH-Resource.
  static_vector<pucch_resource, MAX_NOF_PUCCH_RESOURCES> pucch_res_list;
  /// \c format1 .. \c format4, which contain the parameters that are common to a given PUCCH Format.
  optional<pucch_common_all_formats> format_1_common_param;
  optional<pucch_common_all_formats> format_2_common_param;
  optional<pucch_common_all_formats> format_3_common_param;
  optional<pucch_common_all_formats> format_4_common_param;

  /// List of \c SchedulingRequestResourceConfig.
  static_vector<scheduling_request_resource_config, MAX_NOF_SR_RESOURCES> sr_res_list;

  /// \c dl-DataToUL-ACK. Values {0..15}.
  static_vector<uint8_t, 8> dl_data_to_ul_ack;
};

} // namespace srsgnb
