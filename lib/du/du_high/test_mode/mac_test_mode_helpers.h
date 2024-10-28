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

#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {
namespace srs_du {

/// Create dummy PDU with BSR.
expected<mac_rx_data_indication> create_test_pdu_with_bsr(slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id);

/// Setters for UCI PDUs based on config values.
mac_uci_pdu create_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg);
mac_uci_pdu create_uci_pdu(const ul_sched_info& pusch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg);

/// Check whether a PUCCH grant and MAC UCI PDU match in C-RNTI, format and expected info.
bool pucch_info_and_uci_ind_match(const pucch_info& pucch, const mac_uci_pdu& uci_ind);

} // namespace srs_du
} // namespace srsran