/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_manager_impl.h"
#include "routines/initial_cu_up_setup_routine.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/gtpu_echo_factory.h"
#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include <future>

using namespace srsran;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsran_assert(cfg.ue_exec_pool != nullptr, "Invalid CU-UP UE executor pool");
  srsran_assert(cfg.io_ul_executor != nullptr, "Invalid CU-UP IO UL executor");
  srsran_assert(cfg.e1ap.e1_conn_client != nullptr, "Invalid E1 connection client");
  srsran_assert(cfg.f1u_gateway != nullptr, "Invalid F1-U connector");
  srsran_assert(cfg.ngu_gw != nullptr, "Invalid N3 gateway");
  srsran_assert(cfg.gtpu_pcap != nullptr, "Invalid GTP-U pcap");
}

void fill_sec_as_config(security::sec_as_config& sec_as_config, const e1ap_security_info& sec_info);

cu_up_manager_impl::cu_up_manager_impl(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  /// > Create and connect upper layers

  // Create GTP-U demux
  gtpu_demux_creation_request demux_msg = {};
  demux_msg.cfg.warn_on_drop            = cfg.n3_cfg.warn_on_drop;
  demux_msg.gtpu_pcap                   = cfg.gtpu_pcap;
  ngu_demux                             = create_gtpu_demux(demux_msg);

  ctrl_exec_mapper = cfg.ue_exec_pool->create_ue_executor_mapper();
  report_error_if_not(ctrl_exec_mapper != nullptr, "Could not create CU-UP executor for control TEID");

  // Create GTP-U echo and register it at demux
  gtpu_echo_creation_message ngu_echo_msg = {};
  ngu_echo_msg.gtpu_pcap                  = cfg.gtpu_pcap;
  ngu_echo_msg.tx_upper                   = &gtpu_gw_adapter;
  ngu_echo                                = create_gtpu_echo(ngu_echo_msg);
  ngu_demux->add_tunnel(
      GTPU_PATH_MANAGEMENT_TEID, ctrl_exec_mapper->dl_pdu_executor(), ngu_echo->get_rx_upper_layer_interface());

  // Connect GTP-U DEMUX to adapter.
  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);

  // Establish new NG-U session and connect the instantiated session to the GTP-U DEMUX adapter, so that the latter
  // is called when new NG-U DL PDUs are received.
  ngu_session = cfg.ngu_gw->create(gw_data_gtpu_demux_adapter);
  if (ngu_session == nullptr) {
    report_error("Unable to allocate the required NG-U network resources");
  }

  // Connect GTPU GW adapter to NG-U session in order to send UL PDUs.
  gtpu_gw_adapter.connect_network_gateway(*ngu_session);

  // Create N3 TEID allocator
  gtpu_allocator_creation_request n3_alloc_msg = {};
  n3_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  n3_teid_allocator                            = create_gtpu_allocator(n3_alloc_msg);

  // Create F1-U TEID allocator
  gtpu_allocator_creation_request f1u_alloc_msg = {};
  f1u_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  f1u_teid_allocator                            = create_gtpu_allocator(f1u_alloc_msg);

  /// > Create e1ap
  // e1ap = create_e1ap(*cfg.e1ap.e1_conn_client, e1ap_cu_up_ev_notifier, *cfg.timers, *cfg.ctrl_executor);
  //  e1ap_cu_up_ev_notifier.connect_cu_up(*this);

  cfg.e1ap.e1ap_conn_mng = e1ap.get();

  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(cfg.net_cfg,
                                        cfg.n3_cfg,
                                        *e1ap,
                                        *cfg.timers,
                                        *cfg.f1u_gateway,
                                        gtpu_gw_adapter,
                                        *ngu_demux,
                                        *n3_teid_allocator,
                                        *f1u_teid_allocator,
                                        *cfg.ue_exec_pool,
                                        *cfg.gtpu_pcap,
                                        logger);

  // Start statistics report timer
  if (cfg.statistics_report_period.count() > 0) {
    statistics_report_timer = cfg.timers->create_unique_timer(*cfg.ctrl_executor);
    statistics_report_timer.set(cfg.statistics_report_period,
                                [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
    statistics_report_timer.run();
  }
}

void cu_up_manager_impl::disconnect()
{
  gw_data_gtpu_demux_adapter.disconnect();
  gtpu_gw_adapter.disconnect();
  // e1ap_cu_up_ev_notifier.disconnect();
}
