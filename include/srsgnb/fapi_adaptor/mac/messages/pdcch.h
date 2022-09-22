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

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/mac/mac_cell_result.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief Maximum number of DCIs per PDCCH PDU.
///
/// \note DCIs are grouped by CORESET, BWP and starting symbol properties of the PDCCH PDU.
static constexpr size_t MAX_NUM_DCIS_PER_PDCCH_PDU = MAX_DL_PDCCH_PDUS_PER_SLOT;

/// Groups the DCI information.
struct dci_info {
  const pdcch_dl_information* parameters;
  const dci_payload*          payload;
};

/// Groups the MAC PDCCH PDU.
struct mac_pdcch_pdu {
  const bwp_configuration*                            bwp_cfg;
  const coreset_configuration*                        coreset_cfg;
  static_vector<dci_info, MAX_NUM_DCIS_PER_PDCCH_PDU> dcis;
};

/// \brief Helper function that converts from a PDCCH MAC PDU to a PDCCH FAPI PDU.
///
/// \param[out] fapi_pdu PDCCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu  MAC PDCCH PDU to convert to FAPI.
void convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu& fapi_pdu, const mac_pdcch_pdu& mac_pdu);

/// \brief Helper function that converts from a PDCCH MAC PDU to a PDCCH FAPI PDU.
///
/// \param[out] builder PDCCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu  MAC PDCCH PDU to convert to FAPI.
void convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu_builder& builder, const mac_pdcch_pdu& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsgnb
