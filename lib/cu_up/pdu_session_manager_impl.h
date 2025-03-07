/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_up_ue_logger.h"
#include "ngu_session_manager.h"
#include "pdu_session.h"
#include "pdu_session_manager.h"
#include "srsran/cu_up/cu_up_config.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/rate_limiting/rate_limiter.h"
#include "srsran/support/timers.h"
#include <map>

namespace srsran::srs_cu_up {

class pdu_session_manager_impl final : public pdu_session_manager_ctrl
{
public:
  pdu_session_manager_impl(ue_index_t                                       ue_index_,
                           std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos_cfg_,
                           const security::sec_as_config&                   security_info_,
                           const n3_interface_config&                       n3_config_,
                           const cu_up_test_mode_config&                    test_mode_config_,
                           cu_up_ue_logger&                                 logger_,
                           uint64_t                                         ue_dl_ambr,
                           unique_timer&                                    ue_inactivity_timer_,
                           timer_factory                                    ue_dl_timer_factory_,
                           timer_factory                                    ue_ul_timer_factory_,
                           timer_factory                                    ue_ctrl_timer_factory_,
                           f1u_cu_up_gateway&                               f1u_gw_,
                           ngu_session_manager&                             ngu_session_mngr_,
                           gtpu_teid_pool&                                  n3_teid_allocator_,
                           gtpu_teid_pool&                                  f1u_teid_allocator_,
                           gtpu_demux_ctrl&                                 gtpu_rx_demux_,
                           task_executor&                                   ue_dl_exec_,
                           task_executor&                                   ue_ul_exec_,
                           task_executor&                                   ue_ctrl_exec_,
                           task_executor&                                   crypto_exec_,
                           dlt_pcap&                                        gtpu_pcap_);

  pdu_session_setup_result        setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session) override;
  pdu_session_modification_result modify_pdu_session(const e1ap_pdu_session_res_to_modify_item& session,
                                                     bool new_ul_tnl_info_required) override;
  void                            remove_pdu_session(pdu_session_id_t pdu_session_id) override;
  size_t                          get_nof_pdu_sessions() override;

  void disconnect_pdu_session(pdu_session_id_t pdu_session_id);
  void disconnect_all_pdu_sessions();

  async_task<void> await_crypto_rx_all_pdu_sessions();
  async_task<void> await_crypto_rx_all_drbs(const std::unique_ptr<pdu_session>& pdu_session);
  async_task<void> await_crypto_drb(const std::unique_ptr<drb_context>& drb);

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
  const n3_interface_config&                               n3_config;
  cu_up_test_mode_config                                   test_mode_config;
  cu_up_ue_logger&                                         logger;
  std::unique_ptr<rate_limiter>                            ue_ambr_limiter;
  unique_timer&                                            ue_inactivity_timer;
  timer_factory                                            ue_dl_timer_factory;
  timer_factory                                            ue_ul_timer_factory;
  timer_factory                                            ue_ctrl_timer_factory;
  gtpu_teid_pool&                                          n3_teid_allocator;
  gtpu_teid_pool&                                          f1u_teid_allocator;
  gtpu_demux_ctrl&                                         gtpu_rx_demux;
  task_executor&                                           ue_dl_exec;
  task_executor&                                           ue_ul_exec;
  task_executor&                                           ue_ctrl_exec;
  task_executor&                                           crypto_exec;
  dlt_pcap&                                                gtpu_pcap;
  f1u_cu_up_gateway&                                       f1u_gw;
  ngu_session_manager&                                     ngu_session_mngr;
  std::map<pdu_session_id_t, std::unique_ptr<pdu_session>> pdu_sessions; // key is pdu_session_id
};

} // namespace srsran::srs_cu_up
