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

namespace srsran {

struct dl_msg_alloc;
struct dl_paging_allocation;
struct sib_information;
struct rar_information;

namespace fapi_adaptor {

class precoding_matrix_mapper;

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu SIB MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu&            fapi_pdu,
                               const sib_information&         mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu SIB MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                               const sib_information&         mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a RAR MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu RAR MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu&            fapi_pdu,
                               const rar_information&         mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a RAR MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu RAR MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                               const rar_information&         mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a UE MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu Grant MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu&            fapi_pdu,
                               const dl_msg_alloc&            mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a UE MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu Grant MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                               const dl_msg_alloc&            mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a Paging MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu Paging MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu&            fapi_pdu,
                               const dl_paging_allocation&    mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

/// \brief Helper function that converts from a Paging MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu Paging MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
/// \param[in] pm_mapper Precoding matrix mapper.
/// \param[in] cell_nof_prbs Number of physical resource blocks of the cell.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                               const dl_paging_allocation&    mac_pdu,
                               unsigned                       nof_csi_pdus,
                               const precoding_matrix_mapper& pm_mapper,
                               unsigned                       cell_nof_prbs);

} // namespace fapi_adaptor
} // namespace srsran
