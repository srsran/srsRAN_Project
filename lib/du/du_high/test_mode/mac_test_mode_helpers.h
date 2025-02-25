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

#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

struct pucch_info;
struct ul_sched_info;

namespace srs_du {

/// Create dummy PDU with BSR.
expected<mac_rx_data_indication>
create_test_pdu_with_bsr(du_cell_index_t cell_index, slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id);

/// Create dummy PDU with rrcSetupComplete.
expected<mac_rx_data_indication> create_test_pdu_with_rrc_setup_complete(du_cell_index_t cell_index,
                                                                         slot_point      sl_rx,
                                                                         rnti_t          test_rnti,
                                                                         harq_id_t       harq_id);

/// Setters for UCI PDUs based on config values.
mac_uci_pdu create_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg);
mac_uci_pdu create_uci_pdu(const ul_sched_info& pusch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg);

/// Check whether a PUCCH grant and MAC UCI PDU match in C-RNTI, format and expected info.
bool pucch_info_and_uci_ind_match(const pucch_info& pucch, const mac_uci_pdu& uci_ind);

} // namespace srs_du
} // namespace srsran
