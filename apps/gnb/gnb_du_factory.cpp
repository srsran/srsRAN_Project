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

#include "gnb_du_factory.h"
#include "gnb_appconfig_translators.h"
#include "helpers/gnb_console_helper.h"
#include "srsran/du/du_factory.h"
#include "srsran/e2/e2_connection_client.h"
#include "srsran/f1ap/du/f1c_connection_client.h"

using namespace srsran;

static du_low_configuration create_du_low_config(const gnb_appconfig&                  params,
                                                 upper_phy_rg_gateway*                 rg_gateway,
                                                 span<task_executor*>                  dl_executors,
                                                 task_executor*                        pucch_executor,
                                                 task_executor*                        pusch_executor,
                                                 task_executor*                        prach_executor,
                                                 task_executor*                        pdsch_codeblock_executor,
                                                 upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier)
{
  du_low_configuration du_lo_cfg{};

  du_lo_cfg.logger = &srslog::fetch_basic_logger("DU");

  du_lo_cfg.dl_proc_cfg.ldpc_encoder_type             = "auto";
  du_lo_cfg.dl_proc_cfg.crc_calculator_type           = "auto";
  du_lo_cfg.dl_proc_cfg.nof_pdsch_codeblock_threads   = params.expert_phy_cfg.nof_pdsch_threads;
  du_lo_cfg.dl_proc_cfg.pdsch_codeblock_task_executor = pdsch_codeblock_executor;

  du_lo_cfg.upper_phy = generate_du_low_config(params);

  // Fill the rest with the parameters.
  upper_phy_config& cfg          = du_lo_cfg.upper_phy.front();
  cfg.rg_gateway                 = rg_gateway;
  cfg.dl_executors               = dl_executors;
  cfg.pucch_executor             = pucch_executor;
  cfg.pusch_executor             = pusch_executor;
  cfg.prach_executor             = prach_executor;
  cfg.rx_symbol_request_notifier = rx_symbol_request_notifier;
  cfg.crc_calculator_type        = "auto";
  cfg.ldpc_rate_dematcher_type   = "auto";
  cfg.ldpc_decoder_type          = "auto";

  return du_lo_cfg;
}

std::vector<std::unique_ptr<du>>
srsran::make_gnb_dus(const gnb_appconfig&                                 gnb_cfg,
                     worker_manager&                                      workers,
                     upper_phy_rg_gateway&                                rg_gateway,
                     upper_phy_rx_symbol_request_notifier&                rx_symbol_request_notifier,
                     srs_du::f1c_connection_client&                       f1c_client_handler,
                     srs_du::f1u_du_gateway&                              f1u_gw,
                     timer_manager&                                       timer_mng,
                     mac_pcap&                                            mac_p,
                     gnb_console_helper&                                  console_helper,
                     e2_connection_client&                                e2_client_handler,
                     std::vector<std::unique_ptr<e2_du_metrics_manager>>& e2_du_metric_managers,
                     metrics_hub&                                         metrics_hub)
{
  // DU cell config
  std::vector<du_cell_config> du_cells = generate_du_cell_config(gnb_cfg);
  console_helper.set_cells(du_cells);

  // Set up metrics hub with DU sources and e2 subscribers if enabled.
  for (unsigned i = 0; i < gnb_cfg.cells_cfg.size(); i++) {
    std::string source_name = "DU " + std::to_string(i);
    unsigned    source_idx  = metrics_hub.add_source(source_name);
    if (gnb_cfg.e2_cfg.enable_du_e2) {
      e2_du_metric_managers.push_back(std::make_unique<e2_du_metrics_manager>());
      auto sub = metrics_hub.add_subscriber(*e2_du_metric_managers.back().get());
      metrics_hub.connect_subscriber_to_source(source_idx, sub);
    }
  }
  // This source will aggregate the metrics from all DU sources.
  unsigned console_source_agg_idx = metrics_hub.add_source("console aggregator");
  auto console_agg_subscriber = metrics_hub.add_subscriber(*metrics_hub.get_source_notifier(console_source_agg_idx));
  // Connecting all DU metric sources to the console subscriber via the aggregator source.
  for (unsigned i = 0; i < du_cells.size(); i++) {
    metrics_hub.connect_subscriber_to_source(i, console_agg_subscriber);
  }
  // Adding console as a subscriber to metrics_hub with the console aggregator as a source.
  auto console_subscriber = metrics_hub.add_subscriber(console_helper.get_metrics_notifier());
  metrics_hub.connect_subscriber_to_source(console_source_agg_idx, console_subscriber);
  std::vector<std::unique_ptr<du>> du_insts;
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create a gNB config with one cell.
    gnb_appconfig tmp_cfg = gnb_cfg;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = gnb_cfg.cells_cfg[i];

    // DU QoS config
    std::map<five_qi_t, du_qos_config> du_qos_cfg = generate_du_qos_config(gnb_cfg);

    du_config                   du_cfg = {};
    std::vector<task_executor*> du_low_dl_exec;
    workers.get_du_low_dl_executors(du_low_dl_exec, i);

    // DU-low configuration.
    du_cfg.du_lo = create_du_low_config(tmp_cfg,
                                        &rg_gateway,
                                        du_low_dl_exec,
                                        workers.upper_pucch_exec[i].get(),
                                        workers.upper_pusch_exec[i].get(),
                                        workers.upper_prach_exec[i].get(),
                                        workers.upper_pdsch_exec[i].get(),
                                        &rx_symbol_request_notifier);
    // DU-high configuration.
    srs_du::du_high_configuration& du_hi_cfg = du_cfg.du_hi;
    du_hi_cfg.exec_mapper                    = &workers.get_du_high_executor_mapper(i);
    du_hi_cfg.f1c_client                     = &f1c_client_handler;
    du_hi_cfg.f1u_gw                         = &f1u_gw;
    du_hi_cfg.phy_adapter                    = nullptr;
    du_hi_cfg.timers                         = &timer_mng;
    du_hi_cfg.cells                          = {du_cells[i]};
    du_hi_cfg.qos                            = du_qos_cfg;
    du_hi_cfg.pcap                           = &mac_p;
    du_hi_cfg.gnb_du_id                      = du_insts.size() + 1;
    du_hi_cfg.gnb_du_name                    = fmt::format("srsdu{}", du_hi_cfg.gnb_du_id);
    du_hi_cfg.du_bind_addr                   = {fmt::format("127.0.0.{}", du_hi_cfg.gnb_du_id)};
    du_hi_cfg.mac_cfg                        = generate_mac_expert_config(gnb_cfg);
    du_hi_cfg.metrics_notifier               = metrics_hub.get_source_notifier(i);
    du_hi_cfg.sched_cfg                      = generate_scheduler_expert_config(gnb_cfg);
    if (gnb_cfg.e2_cfg.enable_du_e2) {
      du_hi_cfg.e2_client            = &e2_client_handler;
      du_hi_cfg.e2ap_config          = generate_e2_config(gnb_cfg);
      du_hi_cfg.e2_du_metric_manager = &(*(e2_du_metric_managers[i].get()));
    }
    if (gnb_cfg.test_mode_cfg.test_ue.rnti != INVALID_RNTI) {
      du_hi_cfg.test_cfg.test_ue = srs_du::du_test_config::test_ue_config{gnb_cfg.test_mode_cfg.test_ue.rnti,
                                                                          gnb_cfg.test_mode_cfg.test_ue.pdsch_active,
                                                                          gnb_cfg.test_mode_cfg.test_ue.pusch_active,
                                                                          gnb_cfg.test_mode_cfg.test_ue.cqi,
                                                                          gnb_cfg.test_mode_cfg.test_ue.ri,
                                                                          gnb_cfg.test_mode_cfg.test_ue.pmi,
                                                                          gnb_cfg.test_mode_cfg.test_ue.i_1_1,
                                                                          gnb_cfg.test_mode_cfg.test_ue.i_1_3,
                                                                          gnb_cfg.test_mode_cfg.test_ue.i_2};
    }
    // FAPI configuration.
    du_cfg.fapi.log_level = gnb_cfg.log_cfg.fapi_level;
    du_cfg.fapi.sector    = i;

    du_insts.push_back(make_du(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  return du_insts;
}
