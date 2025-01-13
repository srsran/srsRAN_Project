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

#include "srsran/ran/pusch/pusch_antenna_ports_mapping.h"

using namespace srsran;

#ifndef SRSRAN_HAS_ENTERPRISE

// Current range of supported number of layers.
static constexpr interval<unsigned, true> nof_layers_range(1, 1);

unsigned srsran::get_pusch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                          bool             transform_precoder,
                                                          dmrs_config_type dmrs_cfg_type,
                                                          dmrs_max_length  dmrs_max_len)
{
  srsran_assert(nof_layers_range.contains(nof_layers),
                "The number of layers (i.e., {}) is out of the range {}.",
                nof_layers,
                nof_layers_range);
  srsran_assert(dmrs_cfg_type == dmrs_config_type::type1, "Only DM-RS type 1 is supported.");
  srsran_assert(dmrs_max_len == dmrs_max_length::len1, "Only DM-RS maximum length 1 is supported.");

  if (transform_precoder) {
    // PHY does not support DM-RS in ports other than 0, hence the selected antenna port value is chosen from Table
    // 7.3.1.1.2-6 in TS 38.212 based on assumption of max. rank 1 and DMRS max. length 1.
    return 0;
  }

  // PHY does not support nof. DMRS CDM groups(s) without data other than 2, hence the selected antenna port value
  // is chosen from Table 7.3.1.1.2-8 in TS 38.212 based on assumption of max. rank 1 and DMRS max. length 1.
  return 2;
}

unsigned srsran::get_pusch_precoding_info_row_index(unsigned                                      nof_layers,
                                                    unsigned                                      max_rank,
                                                    srs_resource_configuration::one_two_four_enum nof_srs_ports,
                                                    bool                                          transform_precoder,
                                                    dmrs_config_type                              dmrs_cfg_type,
                                                    dmrs_max_length                               dmrs_max_len,
                                                    unsigned                                      tpmi)
{
  static constexpr interval<unsigned, true> max_rank_range(1, 4);
  srsran_assert(
      max_rank_range.contains(max_rank), "Maximum rank (i.e., {}) is out of the range {}.", max_rank, max_rank_range);
  srsran_assert(nof_layers_range.contains(nof_layers),
                "The number of layers (i.e., {}) is out of the range {}.",
                nof_layers,
                nof_layers_range);
  srsran_assert(nof_layers <= max_rank,
                "The number of layers (i.e., {}) must not exceed the maximum rank (i.e., {}).",
                nof_layers,
                max_rank);
  srsran_assert(dmrs_cfg_type == dmrs_config_type::type1, "Only DM-RS type 1 is supported.");
  srsran_assert(dmrs_max_len == dmrs_max_length::len1, "Only DM-RS maximum length 1 is supported.");

  // TS38.212 Tables 7.3.1.1.2-2 and 7.3.1.1.2-3.
  if ((nof_srs_ports == srs_resource_configuration::one_two_four_enum::four) && !transform_precoder &&
      (dmrs_cfg_type == dmrs_config_type::type1) && (dmrs_max_len == dmrs_max_length::len1)) {
    srsran_assert(tpmi < 28, "PUSCH TPMI, (i.e., {}) must be lower than 28.", tpmi);

    // Direct TPMI to row index for maximum rank 1.
    if (max_rank == 1) {
      return tpmi;
    }

    // TPMI [0, 3] to row index [0, 3]
    if (tpmi < 4) {
      return tpmi;
    }

    // TPMI [4, 11] to row index [12, 19]
    if (tpmi < 12) {
      return tpmi + 8;
    }

    // TPMI [12, 27] to row index [32, 47]
    if (tpmi < 28) {
      return tpmi + 20;
    }
  }

  // TS38.212 Tables 7.3.1.1.2-4 and 7.3.1.1.2-5.
  if ((nof_srs_ports == srs_resource_configuration::one_two_four_enum::two) && !transform_precoder &&
      (dmrs_cfg_type == dmrs_config_type::type1) && (dmrs_max_len == dmrs_max_length::len1)) {
    srsran_assert(tpmi < 7, "PUSCH TPMI, (i.e., {}) must be lower than 9.", tpmi);

    // Direct TPMI to row index for maximum rank 1.
    if (max_rank == 1) {
      return tpmi;
    }

    // TPMI [0, 1] to row index [0, 1]
    if (tpmi < 2) {
      return tpmi;
    }

    // TPMI [2, 5] to row index [3, 6]
    if (tpmi < 6) {
      return tpmi + 1;
    }
  }

  // Unhandled case.
  return 0;
}
#endif // SRSRAN_HAS_ENTERPRISE
