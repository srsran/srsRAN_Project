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

unsigned srsran::get_pusch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                          bool             transform_precoder,
                                                          dmrs_config_type dmrs_cfg_type,
                                                          dmrs_max_length  dmrs_max_len)
{
  srsran_assert(nof_layers == 1, "The number of layers (i.e., {}) must be one.", nof_layers);
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

unsigned srsran::get_pusch_precoding_info_row_index(unsigned nof_layers,
                                                    unsigned max_rank,
                                                    srs_resource_configuration::one_two_four_enum /* nof_srs_ports */,
                                                    unsigned tpmi)
{
  srsran_assert(nof_layers == 1, "The number of layers (i.e., {}) must be one.", nof_layers);
  srsran_assert(max_rank == 1, "The maximum rank (i.e., {}) must be one.", max_rank);

  return tpmi;
}
#endif // SRSRAN_HAS_ENTERPRISE
