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

#include "cu_up_ue_logger.h"
#include "pdu_session.h"
#include "pdu_session_manager.h"
#include "srsran/cu_up/cu_up_configuration.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/support/timers.h"
#include <map>

namespace srsran {

namespace srs_cu_up {

class pdu_session_manager_impl final : public pdu_session_manager_ctrl
{
public:
  pdu_session_manager_impl(ue_index_t                                       ue_index_,
                           std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos_cfg_,
                           const security::sec_as_config&                   security_info_,
                           network_interface_config&                        net_config_,
                           n3_interface_config&                             n3_config_,
                           cu_up_ue_logger&                                 logger_,
                           unique_timer&                                    ue_inactivity_timer_,
                           timer_factory                                    ue_dl_timer_factory_,
                           timer_factory                                    ue_ul_timer_factory_,
                           timer_factory                                    ue_ctrl_timer_factory_,
                           f1u_cu_up_gateway&                               f1u_gw_,
                           gtpu_teid_pool&                                  f1u_teid_allocator_,
                           gtpu_tunnel_tx_upper_layer_notifier&             gtpu_tx_notifier_,
                           gtpu_demux_ctrl&                                 gtpu_rx_demux_,
                           task_executor&                                   ue_dl_exec_,
                           task_executor&                                   ue_ul_exec_,
                           task_executor&                                   ue_ctrl_exec_,
                           dlt_pcap&                                        gtpu_pcap_);

  pdu_session_setup_result        setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session) override;
  pdu_session_modification_result modify_pdu_session(const e1ap_pdu_session_res_to_modify_item& session,
                                                     bool new_ul_tnl_info_required) override;
  void                            remove_pdu_session(pdu_session_id_t pdu_session_id) override;
  size_t                          get_nof_pdu_sessions() override;

  /// \brief Function used to allocate a local NG-U TEID
  /// This function allocates a new TEID based on the UE id, and PDU session ID.
  /// The allocation should look like this,
  ///   |X X X X | 0..3
  ///   |U U P P | 4..7
  /// where X is reserved, U are the bytes for UE id and P is the byte for PDU session Id.
  gtpu_teid_t allocate_local_teid(pdu_session_id_t pdu_session_id);

private:
  drb_setup_result handle_drb_to_setup_item(pdu_session&                         new_session,
                                            const e1ap_drb_to_setup_item_ng_ran& drb_to_setup);

  ue_index_t                                               ue_index;
  const std::map<five_qi_t, srs_cu_up::cu_up_qos_config>   qos_cfg;
  const security::sec_as_config&                           security_info;
  network_interface_config&                                net_config;
  n3_interface_config&                                     n3_config;
  cu_up_ue_logger&                                         logger;
  unique_timer&                                            ue_inactivity_timer;
  timer_factory                                            ue_dl_timer_factory;
  timer_factory                                            ue_ul_timer_factory;
  timer_factory                                            ue_ctrl_timer_factory;
  gtpu_tunnel_tx_upper_layer_notifier&                     gtpu_tx_notifier;
  gtpu_teid_pool&                                          f1u_teid_allocator;
  gtpu_demux_ctrl&                                         gtpu_rx_demux;
  task_executor&                                           ue_dl_exec;
  task_executor&                                           ue_ul_exec;
  task_executor&                                           ue_ctrl_exec;
  dlt_pcap&                                                gtpu_pcap;
  f1u_cu_up_gateway&                                       f1u_gw;
  std::map<pdu_session_id_t, std::unique_ptr<pdu_session>> pdu_sessions; // key is pdu_session_id
};

} // namespace srs_cu_up

} // namespace srsran
