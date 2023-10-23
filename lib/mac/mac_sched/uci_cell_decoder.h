/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "rlf_detector.h"
#include "srsran/du_high/rnti_value_table.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

class uci_cell_decoder
{
  /// \brief Size, in number of slots, of the ring buffer used to store the pending UCIs to be decoded. This size
  /// should account for potential latencies in the PHY in forwarding the decoded UCI to the MAC.
  static constexpr size_t MAX_GRID_SIZE = 80;

public:
  uci_cell_decoder(const sched_cell_configuration_request_message& cell_cfg,
                   const du_rnti_table&                            rnti_table,
                   rlf_detector&                                   rlf_dt);

  /// \brief Store information relative to expected UCIs to be decoded.
  void store_uci(slot_point uci_sl, span<const pucch_info> scheduled_pucchs);

  /// \brief Decode received MAC UCI indication and convert it to scheduler UCI indication.
  uci_indication decode_uci(const mac_uci_indication_message& msg);

private:
  struct uci_context {
    rnti_t                   rnti = INVALID_RNTI;
    csi_report_configuration csi_rep_cfg;
  };

  static size_t to_grid_index(slot_point slot) { return slot.to_uint() % MAX_GRID_SIZE; }

  const du_rnti_table&  rnti_table;
  du_cell_index_t       cell_index;
  rlf_detector&         rlf_handler;
  srslog::basic_logger& logger;

  std::array<static_vector<uci_context, MAX_PUCCH_PDUS_PER_SLOT>, MAX_GRID_SIZE> expected_uci_report_grid;
};

} // namespace srsran
