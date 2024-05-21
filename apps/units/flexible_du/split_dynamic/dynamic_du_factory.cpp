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

#include "dynamic_du_factory.h"
#include "apps/gnb/gnb_appconfig_translators.h"
#include "apps/services/console_helper.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "srsran/du/du_factory.h"
#include "srsran/e2/e2_connection_client.h"
#include "srsran/f1ap/du/f1c_connection_client.h"
#include "srsran/pcap/rlc_pcap.h"

using namespace srsran;

static du_low_configuration create_du_low_config(const du_high_unit_config&            du_high,
                                                 const du_low_unit_config&             params,
                                                 upper_phy_rg_gateway*                 rg_gateway,
                                                 span<task_executor*>                  dl_executors,
                                                 task_executor*                        pucch_executor,
                                                 task_executor*                        pusch_executor,
                                                 task_executor*                        pusch_decoder_executor,
                                                 task_executor*                        prach_executor,
                                                 task_executor*                        srs_executor,
                                                 task_executor*                        pdsch_codeblock_executor,
                                                 upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier)
{
  du_low_configuration du_lo_cfg{};

  du_lo_cfg.logger = &srslog::fetch_basic_logger("DU");

  du_lo_cfg.dl_proc_cfg.ldpc_encoder_type   = "auto";
  du_lo_cfg.dl_proc_cfg.crc_calculator_type = "auto";

  const du_low_unit_expert_threads_config& upper_phy_threads_cfg = params.expert_execution_cfg.threads;

  if ((upper_phy_threads_cfg.pdsch_processor_type == "lite") ||
      ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads == 1))) {
    du_lo_cfg.dl_proc_cfg.pdsch_processor.emplace<pdsch_processor_lite_configuration>();
  } else if ((upper_phy_threads_cfg.pdsch_processor_type == "concurrent") ||
             ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads > 1))) {
    pdsch_processor_concurrent_configuration pdsch_proc_config;
    pdsch_proc_config.nof_pdsch_codeblock_threads = upper_phy_threads_cfg.nof_dl_threads;
    pdsch_proc_config.max_nof_simultaneous_pdsch =
        (MAX_UE_PDUS_PER_SLOT + 1) * params.expert_phy_cfg.max_processing_delay_slots;
    pdsch_proc_config.pdsch_codeblock_task_executor = pdsch_codeblock_executor;
    du_lo_cfg.dl_proc_cfg.pdsch_processor.emplace<pdsch_processor_concurrent_configuration>(pdsch_proc_config);
  } else if (upper_phy_threads_cfg.pdsch_processor_type == "generic") {
    du_lo_cfg.dl_proc_cfg.pdsch_processor.emplace<pdsch_processor_generic_configuration>();
  } else {
    srsran_assert(false, "Invalid PDSCH processor type {}.", upper_phy_threads_cfg.pdsch_processor_type);
  }
  du_lo_cfg.dl_proc_cfg.nof_concurrent_threads = upper_phy_threads_cfg.nof_dl_threads;

  du_lo_cfg.upper_phy = generate_du_low_config(du_high, params);

  // Fill the rest with the parameters.
  upper_phy_config& cfg          = du_lo_cfg.upper_phy.front();
  cfg.rg_gateway                 = rg_gateway;
  cfg.dl_executors               = dl_executors;
  cfg.pucch_executor             = pucch_executor;
  cfg.pusch_executor             = pusch_executor;
  cfg.pusch_decoder_executor     = pusch_decoder_executor;
  cfg.prach_executor             = prach_executor;
  cfg.srs_executor               = srs_executor;
  cfg.rx_symbol_request_notifier = rx_symbol_request_notifier;
  cfg.crc_calculator_type        = "auto";
  cfg.ldpc_rate_dematcher_type   = "auto";
  cfg.ldpc_decoder_type          = "auto";

  return du_lo_cfg;
}

std::vector<std::unique_ptr<du>> srsran::make_gnb_dus(const gnb_appconfig&                  gnb_cfg,
                                                      const dynamic_du_unit_config&         dyn_du_cfg,
                                                      span<du_cell_config>                  du_cells,
                                                      worker_manager&                       workers,
                                                      upper_phy_rg_gateway&                 rg_gateway,
                                                      upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
                                                      srs_du::f1c_connection_client&        f1c_client_handler,
                                                      srs_du::f1u_du_gateway&               f1u_gw,
                                                      timer_manager&                        timer_mng,
                                                      mac_pcap&                             mac_p,
                                                      rlc_pcap&                             rlc_p,
                                                      console_helper&                       console_helper,
                                                      e2_connection_client&                 e2_client_handler,
                                                      e2_metric_connector_manager&          e2_metric_connectors,
                                                      rlc_metrics_notifier&                 rlc_json_metrics,
                                                      metrics_hub&                          metrics_hub)
{
  const du_high_unit_config& du_high  = dyn_du_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_low   = dyn_du_cfg.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = dyn_du_cfg.fapi_cfg;

  // DU cell config
  console_helper.set_cells(du_cells);

  // Set up sources for the DU Scheruler UE metrics and add them to metric hub.
  for (unsigned i = 0; i < du_cells.size(); i++) {
    std::string source_name = "DU " + std::to_string(i);
    auto        source      = std::make_unique<scheduler_ue_metrics_source>(source_name);
    metrics_hub.add_source(std::move(source));

    // Get DU Scheduler UE metrics source pointer.
    scheduler_ue_metrics_source* sched_source = metrics_hub.get_scheduler_ue_metrics_source(source_name);
    if (sched_source == nullptr) {
      continue;
    }

    // Connect Console Aggregator to DU Scheduler UE metrics.
    sched_source->add_subscriber(console_helper.get_stdout_metrics_notifier());

    // Connect JSON metrics reporter to DU Scheduler UE metrics.
    if (gnb_cfg.metrics_cfg.enable_json_metrics) {
      sched_source->add_subscriber(console_helper.get_json_metrics_notifier());
    }

    // Connect E2 agent to DU Scheduler UE metrics.
    if (gnb_cfg.e2_cfg.enable_du_e2) {
      sched_source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
    }
  }

  std::vector<std::unique_ptr<du>> du_insts;
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create a gNB config with one cell.
    du_high_unit_config tmp_cfg = du_high;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_high.cells_cfg[i];

    du_config                   du_cfg = {};
    std::vector<task_executor*> du_low_dl_exec;
    workers.get_du_low_dl_executors(du_low_dl_exec, i);

    // DU-low configuration.
    du_cfg.du_lo = create_du_low_config(tmp_cfg,
                                        du_low,
                                        &rg_gateway,
                                        du_low_dl_exec,
                                        workers.upper_pucch_exec[i],
                                        workers.upper_pusch_exec[i],
                                        workers.upper_pusch_decoder_exec[i],
                                        workers.upper_prach_exec[i],
                                        workers.upper_srs_exec[i],
                                        workers.upper_pdsch_exec[i],
                                        &rx_symbol_request_notifier);
    // DU-high configuration.
    srs_du::du_high_configuration& du_hi_cfg = du_cfg.du_hi;
    du_hi_cfg.exec_mapper                    = &workers.get_du_high_executor_mapper(i);
    du_hi_cfg.f1c_client                     = &f1c_client_handler;
    du_hi_cfg.f1u_gw                         = &f1u_gw;
    du_hi_cfg.phy_adapter                    = nullptr;
    du_hi_cfg.timers                         = &timer_mng;
    du_hi_cfg.cells                          = {du_cells[i]};
    du_hi_cfg.srbs                           = generate_du_srb_config(du_high);
    du_hi_cfg.qos                            = generate_du_qos_config(du_high);
    du_hi_cfg.mac_p                          = &mac_p;
    du_hi_cfg.rlc_p                          = &rlc_p;
    du_hi_cfg.gnb_du_id                      = du_insts.size() + 1;
    du_hi_cfg.gnb_du_name                    = fmt::format("srsdu{}", du_hi_cfg.gnb_du_id);
    du_hi_cfg.du_bind_addr =
        transport_layer_address::create_from_string(fmt::format("127.0.0.{}", du_hi_cfg.gnb_du_id));
    du_hi_cfg.mac_cfg = generate_mac_expert_config(du_high);
    // Assign different initial C-RNTIs to different DUs.
    du_hi_cfg.mac_cfg.initial_crnti     = to_rnti(0x4601 + (0x1000 * du_insts.size()));
    du_hi_cfg.sched_ue_metrics_notifier = metrics_hub.get_scheduler_ue_metrics_source("DU " + std::to_string(i));
    du_hi_cfg.sched_cfg                 = generate_scheduler_expert_config(du_high);

    // Connect RLC metrics to sinks, if required
    if (du_high.metrics.rlc.json_enabled || gnb_cfg.e2_cfg.enable_du_e2) {
      // This source aggregates the RLC metrics from all DRBs in a single DU.
      std::string source_name = "rlc_metric_aggr_du_" + std::to_string(i);
      auto        rlc_source  = std::make_unique<rlc_metrics_source>(source_name);

      if (du_high.metrics.rlc.json_enabled) {
        // Connect JSON metrics plotter to RLC metric source.
        rlc_source->add_subscriber(rlc_json_metrics);
      }
      if (gnb_cfg.e2_cfg.enable_du_e2) {
        // Connect E2 agent to RLC metric source.
        du_hi_cfg.e2_client          = &e2_client_handler;
        du_hi_cfg.e2ap_config        = generate_e2_config(gnb_cfg, du_high);
        du_hi_cfg.e2_du_metric_iface = &(e2_metric_connectors.get_e2_du_metrics_interface(i));
        rlc_source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
      }
      // Pass RLC metric source to the DU high.
      metrics_hub.add_source(std::move(rlc_source));
      du_hi_cfg.rlc_metrics_notif = metrics_hub.get_rlc_metrics_source(source_name);
    }

    // Configure test mode
    if (du_high.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
      du_hi_cfg.test_cfg.test_ue =
          srs_du::du_test_config::test_ue_config{du_high.test_mode_cfg.test_ue.rnti,
                                                 du_high.test_mode_cfg.test_ue.nof_ues,
                                                 du_high.test_mode_cfg.test_ue.auto_ack_indication_delay,
                                                 du_high.test_mode_cfg.test_ue.pdsch_active,
                                                 du_high.test_mode_cfg.test_ue.pusch_active,
                                                 du_high.test_mode_cfg.test_ue.cqi,
                                                 du_high.test_mode_cfg.test_ue.ri,
                                                 du_high.test_mode_cfg.test_ue.pmi,
                                                 du_high.test_mode_cfg.test_ue.i_1_1,
                                                 du_high.test_mode_cfg.test_ue.i_1_3,
                                                 du_high.test_mode_cfg.test_ue.i_2};
    }
    // FAPI configuration.
    du_cfg.fapi.log_level = fapi_cfg.fapi_level;
    du_cfg.fapi.sector    = i;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      du_cfg.fapi.executor.emplace(workers.fapi_exec[i]);
    } else {
      report_error_if_not(workers.fapi_exec[i] == nullptr,
                          "FAPI buffered worker created for a cell with no MAC delay configured");
    }

    // As the temporal configuration contains only once cell, pick the data from that cell.
    du_cfg.fapi.prach_ports        = tmp_cfg.cells_cfg.front().cell.prach_cfg.ports;
    du_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;

    du_insts.push_back(make_du(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  return du_insts;
}
