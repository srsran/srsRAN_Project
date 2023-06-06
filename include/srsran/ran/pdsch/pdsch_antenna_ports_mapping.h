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

#include "srsran/adt/static_vector.h"
#include "srsran/scheduler/config/dmrs.h"

namespace srsran {

/// Antenna ports mapping information for PDSCH when using DCI format 1_1. See TS 38.212 Tables 7.3.1.2.2-1/2/3/4.
struct pdsch_antenna_ports_mapping {
  /// Number of DMRS CDM group(s) without data.
  unsigned nof_dmrs_cdm_groups_without_data;
  /// DMRS ports.
  std::vector<unsigned> dmrs_ports;
  /// Number of front-load symbols.
  unsigned nof_front_load_symbols = 1;
};

/// \brief Returns the PDSCH antenna ports mapping corresponding to the row in Tables 7.3.1.2.2-1/2/3/4, TS 38.212.
/// \param[in] nof_layers Number of layers.
/// \param[in] nof_dl_antenna_ports Number of DL antenna ports supported by gNB.
/// \param[in] dmrs_cfg_type PDSCH DMRS configuration type.
/// \param[in] dmrs_max_len PDSCH DMRS maximum number of OFDM symbols.
/// \return The PDSCH antenna ports mapping corresponding to the row in Tables 7.3.1.2.2-1/2/3/4, TS 38.212 based on
///         DMRS configuration, nof. layers etc. If the given input configuration does not match any of the rows in the
///         table, the function triggers an asserts. But if input configuration matches more than one row in the table,
///         the first matching row information is returned.
pdsch_antenna_ports_mapping get_pdsch_antenna_port_mapping(unsigned         nof_layers,
                                                           unsigned         nof_dl_antenna_ports,
                                                           dmrs_config_type dmrs_cfg_type,
                                                           dmrs_max_length  dmrs_max_len);

/// \brief Returns the PDSCH antenna ports mapping row index in Tables 7.3.1.2.2-1/2/3/4, TS 38.212.
/// \param[in] nof_layers Number of layers.
/// \param[in] nof_dl_antenna_ports Number of DL antenna ports supported by gNB.
/// \param[in] dmrs_cfg_type PDSCH DMRS configuration type.
/// \param[in] dmrs_max_len PDSCH DMRS maximum number of OFDM symbols.
/// \return The PDSCH antenna ports mapping row index in Tables 7.3.1.2.2-1/2/3/4, TS 38.212 based on DMRS
///         configuration, nof. layers etc. If the given input configuration does not match any of the rows in the
///         table, the function triggers an asserts. But if input configuration matches more than one row in the
///         table,the first matching row index is returned.
unsigned get_pdsch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                  unsigned         nof_dl_antenna_ports,
                                                  dmrs_config_type dmrs_cfg_type,
                                                  dmrs_max_length  dmrs_max_len);

} // namespace srsran
