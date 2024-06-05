/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_builder.h"
#include "apps/services/worker_manager.h"
#include "cu_up_unit_config.h"
#include "cu_up_unit_config_translators.h"
#include "cu_up_wrapper.h"
#include "srsran/cu_up/cu_up_factory.h"

using namespace srsran;

std::unique_ptr<srs_cu_up::cu_up_interface> srsran::build_cu_up(const cu_up_unit_config&         unit_cfg,
                                                                worker_manager&                  workers,
                                                                srs_cu_up::e1_connection_client& e1_conn_client,
                                                                f1u_cu_up_gateway&               f1u_gateway,
                                                                dlt_pcap&                        gtpu_pcap,
                                                                timer_manager&                   timers,
                                                                io_broker&                       io_brk)
{
  srs_cu_up::cu_up_configuration config = generate_cu_up_config(unit_cfg);
  config.ctrl_executor                  = workers.cu_up_ctrl_exec;
  config.cu_up_e2_exec                  = workers.cu_up_e2_exec;
  config.ue_exec_pool                   = workers.cu_up_exec_mapper.get();
  config.io_ul_executor                 = workers.cu_up_io_ul_exec; // Optionally select separate exec for UL IO
  config.e1ap.e1_conn_client            = &e1_conn_client;
  config.f1u_gateway                    = &f1u_gateway;
  config.gtpu_pcap                      = &gtpu_pcap;
  config.timers                         = &timers;
  config.qos                            = generate_cu_up_qos_config(unit_cfg);

  // Create NG-U gateway.
  std::unique_ptr<srs_cu_up::ngu_gateway> ngu_gw;
  if (not unit_cfg.upf_cfg.no_core) {
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = config.net_cfg.n3_bind_addr;
    ngu_gw_config.bind_port                  = config.net_cfg.n3_bind_port;
    ngu_gw_config.bind_interface             = config.net_cfg.n3_bind_interface;
    ngu_gw_config.rx_max_mmsg                = config.net_cfg.n3_rx_max_mmsg;
    ngu_gw = srs_cu_up::create_udp_ngu_gateway(ngu_gw_config, io_brk, *workers.cu_up_io_ul_exec);
  } else {
    ngu_gw = srs_cu_up::create_no_core_ngu_gateway();
  }
  config.ngu_gw = ngu_gw.get();

  return std::make_unique<cu_up_wrapper>(std::move(ngu_gw), create_cu_up(config));
}
