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

#include "cell_group_config.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/pucch/pucch_configuration.h"

namespace srsran {
namespace srs_du {

enum class nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

inline unsigned format1_cp_step_to_uint(nof_cyclic_shifts opt)
{
  return static_cast<unsigned>(opt);
}

struct pucch_params_option1 {
  /// Maximum number of RBs for PUCCH resources.
  unsigned max_pucch_f1_rbs;
  unsigned max_pucch_f2_rbs;
  /// Number of symbols per slot for PUCCH resources.
  bounded_integer<unsigned, 4, 14> pucch_symbols{14};
  // Format 1.
  nof_cyclic_shifts                format1_cp{nof_cyclic_shifts::no_cyclic_shift};
  bool                             f1_occ_supported{false};
  bounded_integer<unsigned, 4, 14> nof_f1_symbols{14};
  bool                             f1_intraslot_freq_hopping{false};
  // Format 2.
  unsigned            max_f2_payload_bits{8};
  max_pucch_code_rate max_f2_code_rate{max_pucch_code_rate::dot_25};
  bool                f2_intraslot_freq_hopping{false};
};

struct pucch_params_option2 {
  /// Mandatory parameters.
  unsigned nof_res_f1;
  unsigned nof_res_f2;
  /// Optional parameters with default values.
  /// PUCCH Format 1.
  nof_cyclic_shifts                format1_cp{nof_cyclic_shifts::no_cyclic_shift};
  bool                             f1_occ_supported{false};
  bounded_integer<unsigned, 4, 14> nof_f1_symbols{14};
  bool                             f1_intraslot_freq_hopping{false};
  /// PUCCH Format 2.
  unsigned            max_f2_payload_bits{8};
  max_pucch_code_rate max_f2_code_rate{max_pucch_code_rate::dot_25};
  bool                f2_intraslot_freq_hopping{false};
};

/// \c PUCCH-Resource, in \c PUCCH-Config, TS 38.331.
struct cell_pucch_resource {
  unsigned                                                                                  cell_res_id;
  unsigned                                                                                  starting_prb;
  optional<unsigned>                                                                        second_hop_prb;
  pucch_format                                                                              format;
  variant<pucch_format_0_cfg, pucch_format_1_cfg, pucch_format_2_3_cfg, pucch_format_4_cfg> format_params;
};

std::vector<cell_pucch_resource>
generate_pucch_resource_list(variant<pucch_params_option1, pucch_params_option2> params, unsigned bwp_size);

} // namespace srs_du
} // namespace srsran