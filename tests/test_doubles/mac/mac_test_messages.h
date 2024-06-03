/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

struct pucch_info;

namespace test_helpers {

/// Creates a CCCH message with the given RNTI.
mac_rx_data_indication
create_ccch_message(slot_point sl_rx, rnti_t rnti, du_cell_index_t du_cell_index = to_du_cell_index(0));

mac_rx_data_indication create_pdu_with_sdu(slot_point sl_rx, rnti_t rnti, lcid_t lcid);

/// \brief Generate MAC UCI PDU from PUCCH information, with all HARQ-ACKs set to ACK, SRs set as not detected and CSI
/// set as 1s.
mac_uci_pdu create_uci_pdu(const pucch_info& pucch);

/// Generate MAC UCI PDU out of a PUSCH UCI PDU indication, with all HARQ-ACKs set to ACK and CSI set as 1s.
mac_uci_pdu create_uci_pdu(rnti_t rnti, const uci_info& pusch_uci);

mac_uci_indication_message create_uci_indication(slot_point sl_rx, span<const pucch_info> pucchs);

std::optional<mac_uci_indication_message> create_uci_indication(slot_point sl_rx, span<const ul_sched_info> puschs);

mac_crc_indication_message create_crc_indication(slot_point sl_rx, rnti_t rnti, harq_id_t h_id);

mac_crc_indication_message create_crc_indication(slot_point sl_rx, span<const ul_sched_info> puschs);

} // namespace test_helpers
} // namespace srsran
