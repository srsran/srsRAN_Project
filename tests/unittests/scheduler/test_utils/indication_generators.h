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

#pragma once

#include "srsran/scheduler/result/pusch_info.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"

namespace srsran {

struct pucch_info;

namespace test_helper {

/// Create a detected PRACH preamble.
rach_indication_message::preamble create_preamble(unsigned preamble_id, rnti_t tc_rnti);

/// Create a RACH indication with one occation and multiple PRACH preambles.
rach_indication_message create_rach_indication(slot_point                                            slot_rx,
                                               const std::vector<rach_indication_message::preamble>& preambles);

/// Create dummy UCI indication based on a PUCCH PDU.
uci_indication create_uci_indication(slot_point uci_sl, du_ue_index_t ue_idx, const pucch_info& pucch_pdu);

/// Create dummy UCI PDU based on a PUCCH PDU.
/// \param ue_idx UE index to set in the UCI PDU.
/// \param pucch_pdu PUCCH PDU based on which the UCI PDU will be created.
/// \param ack_set If set, it overrides the ACK/NACK bits in the PUCCH PDU. If not set, we assume ACK for all bits,
/// except for SR-only PUCCHs, which are set to DTX.
/// \return Created UCI PDU.
uci_indication::uci_pdu create_uci_indication_pdu(du_ue_index_t                             ue_idx,
                                                  const pucch_info&                         pucch_pdu,
                                                  std::optional<mac_harq_ack_report_status> ack_set = std::nullopt);

/// Create dummy UCI PDU based on a PUSCH UCI PDU.
uci_indication::uci_pdu create_uci_indication_pdu(rnti_t rnti, du_ue_index_t ue_idx, const uci_info& uci);

/// Create dummy CRC PDU indication based on a PUSCH PDU.
ul_crc_pdu_indication create_crc_pdu_indication(const ul_sched_info& ul_grant);

} // namespace test_helper

} // namespace srsran
