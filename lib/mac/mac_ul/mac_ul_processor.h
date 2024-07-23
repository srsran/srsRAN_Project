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

#include "../mac_config_interfaces.h"
#include "mac_ul_ue_manager.h"
#include "pdu_rx_handler.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_config.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/support/async/execute_on.h"

namespace srsran {

struct mac_ul_config {
  task_executor&                 ctrl_exec;
  du_high_ue_executor_mapper&    ue_exec_mapper;
  mac_ul_ccch_notifier&          ul_ccch_notifier;
  mac_scheduler_ce_info_handler& sched;
  du_rnti_table&                 rnti_table;
  mac_pcap&                      pcap;
};

class mac_ul_processor final : public mac_ul_configurator, public mac_pdu_handler
{
public:
  mac_ul_processor(const mac_ul_config& cfg_);

  async_task<bool> add_ue(const mac_ue_create_request& request) override;

  async_task<bool> addmod_bearers(du_ue_index_t                                  ue_index,
                                  const std::vector<mac_logical_channel_config>& ul_logical_channels) override;

  async_task<bool> remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem) override;

  async_task<void> remove_ue(const mac_ue_delete_request& msg) override;

  bool flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer ccch_pdu) override;

  void handle_ue_config_applied(du_ue_index_t ue_index) override;

  /// Handles FAPI Rx_Data.Indication.
  /// The PDUs contained in the Rx_Data.Indication are dispatched to different executors, depending on their RNTI.
  void handle_rx_data_indication(mac_rx_data_indication msg) override;

private:
  const mac_ul_config   cfg;
  srslog::basic_logger& logger;

  mac_ul_ue_manager ue_manager;

  /// Object that handles incoming UL MAC PDUs.
  pdu_rx_handler pdu_handler;
};

} // namespace srsran
