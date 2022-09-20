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
#include "srsgnb/ran/sr_configuration.h"
#include <cstdint>
#include <vector>

namespace srsgnb {

/// Options for \c occ-Length in \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
enum class pucch_f4_occ_len { n2, n4 };

/// Options for \c occ-Index in \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
enum class pucch_f4_occ_idx { n0, n1, n2, n4 };

/// \c PUCCH-FormatConfig, in \c PUCCH-Config, TS 38.331. It describes the parameters that are common for a given PUCCH
/// format.
struct pucch_common_all_formats {
  enum class max_code_rate { dot_08, dot_15, dot_25, dot_35, dot_45, dot_60, dot_80 };
  enum class num_of_slots { n2, n4, n8 };

  bool          interslot_freq_hop;
  bool          additional_dmrs;
  max_code_rate max_c_rate;
  num_of_slots  nof_slots;
  bool          pi_2_bpsk;
  bool          simultaneous_harq_ack_csi;
};

/// \c PUCCH-format0, in \c PUCCH-Config, TS 38.331.
struct pucch_format_0_cfg {
  uint8_t initial_cyclic_shift;
  uint8_t nof_symbols;
  uint8_t starting_sym_idx;
};

/// \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
struct pucch_format_1_cfg {
  uint8_t initial_cyclic_shift;
  uint8_t nof_symbols;
  uint8_t starting_sym_idx;
  uint8_t time_domain_occ;
};

/// \c PUCCH-format2, in \c PUCCH-Config, TS 38.331.
struct pucch_format_2_3_cfg {
  uint8_t nof_prbs;
  uint8_t nof_symbols;
  uint8_t starting_sym_idx;
};

/// \c PUCCH-format4, in \c PUCCH-Config, TS 38.331.
struct pucch_format_4_cfg {
  uint8_t          nof_symbols;
  pucch_f4_occ_len occ_length;
  pucch_f4_occ_idx occ_index;
  uint8_t          starting_sym_idx;
};

/// \c PUCCH-Resource, in \c PUCCH-Config, TS 38.331.
struct pucch_resource {
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
  /// \c resourceList.
  std::vector<unsigned> pucch_res_id_list;
  /// \c maxPayloadSize.
  optional<unsigned> max_payload_size;
};

/// \c PUCCH-Config, TS 38.311.
struct pucch_config {
  /// \c PUCCH-ResourceSet, from 0 to 3.
  // NOTE: PUCCH resource set ID 0 can only contain PUCCH format 0 and 1.
  pucch_resource_set pucch_res_set_0;
  pucch_resource_set pucch_res_set_1;
  pucch_resource_set pucch_res_set_2;
  pucch_resource_set pucch_res_set_3;
  /// List of \c PUCCH-Resource.
  std::vector<pucch_resource> pucch_res_list;
  /// \c format1 .. \c format4, which contain the parameters that are common to a given PUCCH Format.
  optional<pucch_common_all_formats> format_1_common_param;
  optional<pucch_common_all_formats> format_2_common_param;
  optional<pucch_common_all_formats> format_3_common_param;
  optional<pucch_common_all_formats> format_4_common_param;

  /// List of \c SchedulingRequestResourceConfig.
  std::vector<scheduling_request_resource_config> sr_res_list;

  /// \c dl-DataToUL-ACK.
  std::vector<unsigned> dl_data_to_ul_ack;
};

} // namespace srsgnb
