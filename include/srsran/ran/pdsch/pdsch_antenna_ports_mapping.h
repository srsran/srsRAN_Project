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

#include "srsran/adt/static_vector.h"
#include "srsran/scheduler/config/dmrs.h"

namespace srsran {

/// Antenna ports mapping information for PDSCH when using DCI format 1_1. See TS 38.212 Tables 7.3.1.2.2-1/2/3/4.
struct pdsch_antenna_ports_mapping {
  /// Number of DMRS CDM group(s) without data.
  unsigned nof_dmrs_cdm_groups_without_data;
  /// Contains list of DMRS ports used.
  /// Example 1: For a cell with 4 DL antenna ports and if the value of DMRS ports list is {0, 3}, then the DMRS ports
  /// (1000 + 0) = 1000 and (1000 + 3) = 1003 are used.
  /// Example 2: For a cell with 2 DL antenna ports and if the value of DMRS ports list is {0, 1}, then the DMRS ports
  /// (1000 + 0) = 1000 and (1000 + 1) = 1001 are used.
  std::vector<unsigned> dmrs_ports;
  /// Number of DMRS front-load symbols. Number of front-load symbols equates to maxLength in DMRS-DownlinkConfig whose
  /// default value is Len1 i.e. 1 symbol.
  unsigned nof_front_load_symbols = 1;
};

/// \brief Returns the PDSCH antenna ports mapping from TS 38.212, tables 7.3.1.2.2-1/2/3/4 based on input
/// configuration.
/// \param[in] nof_layers Number of layers.
/// \param[in] nof_dl_antenna_ports Number of DL antenna ports supported by gNB.
/// \param[in] dmrs_cfg_type PDSCH DMRS configuration type.
/// \param[in] dmrs_max_len PDSCH DMRS maximum number of OFDM symbols.
/// \param[in] are_both_cws_enabled Flag indicating whether Codeword 0 and Codeword 1 are enabled or not.
/// \return The PDSCH antenna ports mapping from TS 38.212, tables 7.3.1.2.2-1/2/3/4 based on DMRS configuration, nof.
///         layers etc. If the given input configuration does not match any of the rows in the table, the function
///         triggers an asserts. But if input configuration matches more than one row in the table, the first matching
///         row information is returned.
pdsch_antenna_ports_mapping get_pdsch_antenna_port_mapping(unsigned         nof_layers,
                                                           unsigned         nof_dl_antenna_ports,
                                                           dmrs_config_type dmrs_cfg_type,
                                                           dmrs_max_length  dmrs_max_len,
                                                           bool             are_both_cws_enabled);

/// \brief Returns the PDSCH antenna ports mapping row index in TS 38.212, tables 7.3.1.2.2-1/2/3/4.
/// \param[in] nof_layers Number of layers.
/// \param[in] nof_dl_antenna_ports Number of DL antenna ports supported by gNB.
/// \param[in] dmrs_cfg_type PDSCH DMRS configuration type.
/// \param[in] dmrs_max_len PDSCH DMRS maximum number of OFDM symbols.
/// \param[in] are_both_cws_enabled Flag indicating whether Codeword 0 and Codeword 1 are enabled or not.
/// \return The PDSCH antenna ports mapping row index from TS 38.212, tables 7.3.1.2.2-1/2/3/4 based on DMRS
///         configuration, nof. layers etc. If the given input configuration does not match any of the rows in the
///         table, the function triggers an asserts. But if input configuration matches more than one row in the
///         table,the first matching row index is returned.
unsigned get_pdsch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                  unsigned         nof_dl_antenna_ports,
                                                  dmrs_config_type dmrs_cfg_type,
                                                  dmrs_max_length  dmrs_max_len,
                                                  bool             are_both_cws_enabled);

} // namespace srsran
