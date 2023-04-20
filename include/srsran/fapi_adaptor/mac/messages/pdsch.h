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

#include "srsran/fapi/message_builders.h"

namespace srsran {

struct dl_msg_alloc;
struct sib_information;
struct rar_information;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu SIB MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const sib_information& mac_pdu, unsigned nof_csi_pdus);

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu SIB MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder,
                               const sib_information&      mac_pdu,
                               unsigned                    nof_csi_pdus);

/// \brief Helper function that converts from a RAR MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu RAR MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const rar_information& mac_pdu, unsigned nof_csi_pdus);

/// \brief Helper function that converts from a RAR MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu RAR MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder,
                               const rar_information&      mac_pdu,
                               unsigned                    nof_csi_pdus);

/// \brief Helper function that converts from a UE MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu Grant MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const dl_msg_alloc& mac_pdu, unsigned nof_csi_pdus);

/// \brief Helper function that converts from a UE MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu Grant MAC PDU.
/// \param[in] nof_csi_pdus Number of CSI-RS PDUs colliding with this PDSCH PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder, const dl_msg_alloc& mac_pdu, unsigned nof_csi_pdus);

} // namespace fapi_adaptor
} // namespace srsran
