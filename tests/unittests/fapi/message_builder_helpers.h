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

#include "srsran/fapi/messages.h"

namespace unittest {
/// Builds and returns a valid CRC.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.8.
srsran::fapi::crc_indication_message build_valid_crc_indication();

/// Builds and returns a valid DL CSI-RS pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.3.
srsran::fapi::dl_csi_rs_pdu build_valid_dl_csi_pdu();

/// Builds and returns a valid DL PDCCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.1.
srsran::fapi::dl_pdcch_pdu build_valid_dl_pdcch_pdu();

/// Builds and returns a valid DL PDSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.2.
srsran::fapi::dl_pdsch_pdu build_valid_dl_pdsch_pdu();

/// Builds and returns a valid DL SSB pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.4.
srsran::fapi::dl_ssb_pdu build_valid_dl_ssb_pdu();

/// Builds and returns a valid DL TTI request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.
srsran::fapi::dl_tti_request_message build_valid_dl_tti_request();

/// Builds and returns a valid ERROR.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_error_indication();

/// Builds and returns a valid OUT_OF_SYNC ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_out_of_sync_error_indication();

/// Builds and returns a valid MSG_INVALID_SFN ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_invalid_sfn_error_indication();

/// Builds and returns a valid MSG_SLOT_ERR ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_msg_slot_error_indication();

/// Builds and returns a valid MSG_TX_ERR ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_tx_err_error_indication();

/// Builds and returns a valid MSG_UL_DCI_ERR ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_ul_dci_err_error_indication();

/// Builds and returns a RACH.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.11.
srsran::fapi::rach_indication_message build_valid_rach_indication();

/// Builds and returns a valid Rx_Data.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.7.
srsran::fapi::rx_data_indication_message build_valid_rx_data_indication();

/// Builds and returns a valid SLOT.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.1.
srsran::fapi::slot_indication_message build_valid_slot_indication();

/// Builds and returns a valid Tx_Data.request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.6.
/// \note Payload pointer is set to \c nullptr.
srsran::fapi::tx_data_request_message build_valid_tx_data_request();

/// Builds and returns a valid transmission precoding and beamforming PDU. Every parameter is within the range defined
/// in SCF-222 v4.0 Section 3.4.2.5.
srsran::fapi::tx_precoding_and_beamforming_pdu build_valid_tx_precoding_and_beamforming_pdu();

/// Builds and returns a valid UCI.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.
srsran::fapi::uci_indication_message build_valid_uci_indication();

/// Builds and returns a valid UCI PUSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.1.
srsran::fapi::uci_pusch_pdu build_valid_uci_pusch_pdu();

/// Builds and returns a valid UCI PUCCH format 0/1 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.2.
srsran::fapi::uci_pucch_pdu_format_0_1 build_valid_uci_pucch_format01_pdu();

/// Builds and returns a valid UCI PUCCH format 2/3/4 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.3.
srsran::fapi::uci_pucch_pdu_format_2_3_4 build_valid_uci_pucch_format234_pdu();

/// Builds and returns a valid UL_DCI.request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.4.
srsran::fapi::ul_dci_request_message build_valid_ul_dci_request();

/// Builds and returns a valid UL PRACH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.1.
srsran::fapi::ul_prach_pdu build_valid_ul_prach_pdu();

/// Builds and returns a valid UL PUCCH format 0 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f0_pdu();

/// Builds and returns a valid UL PUCCH format 1 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f1_pdu();

/// Builds and returns a valid UL PUCCH format 2 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f2_pdu();

/// Builds and returns a valid UL PUCCH format 3 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f3_pdu();

/// Builds and returns a valid UL PUCCH format 4 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f4_pdu();

/// Builds and returns a valid UL PUSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.2.
srsran::fapi::ul_pusch_pdu build_valid_ul_pusch_pdu();

/// Builds and returns a valid UL_TTI.request. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.
srsran::fapi::ul_tti_request_message build_valid_ul_tti_request();

} // namespace unittest
