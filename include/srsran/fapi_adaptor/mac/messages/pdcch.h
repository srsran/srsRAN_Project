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

#include "srsran/fapi/message_builders.h"
#include "srsran/mac/mac_cell_result.h"

namespace srsran {
namespace fapi_adaptor {

class precoding_matrix_mapper;

/// Collection of downlink DCIs that share the same BWP, CORESET and starting symbol.
struct mac_pdcch_pdu {
  /// Groups the DCI information.
  struct dci_info {
    dci_info(const dci_context_information* info_, const dci_payload* payload_) : info(info_), payload(payload_) {}
    const dci_context_information* info;
    const dci_payload*             payload;
  };

  const bwp_configuration*                                  bwp_cfg;
  const coreset_configuration*                              coreset_cfg;
  unsigned                                                  start_symbol;
  static_vector<dci_info, fapi::MAX_NUM_DCIS_PER_PDCCH_PDU> dcis;
};

/// \brief Helper function that converts from a PDCCH MAC PDU to a PDCCH FAPI PDU.
///
/// \param[out] fapi_pdu PDCCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu  MAC PDCCH PDU to convert to FAPI.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Cell number of PRBs.
void convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu&            fapi_pdu,
                               const mac_pdcch_pdu&           mac_pdu,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a PDCCH MAC PDU to a PDCCH FAPI PDU.
///
/// \param[out] builder PDCCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu  MAC PDCCH PDU to convert to FAPI.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Cell number of PRBs.
void convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu_builder&    builder,
                               const mac_pdcch_pdu&           mac_pdu,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

} // namespace fapi_adaptor
} // namespace srsran
