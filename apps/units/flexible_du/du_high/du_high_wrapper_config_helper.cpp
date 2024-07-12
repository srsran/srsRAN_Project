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

#include "du_high_wrapper_config_helper.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/services/metrics_hub.h"
#include "apps/services/metrics_log_helper.h"
#include "apps/services/metrics_plotter_json.h"
#include "apps/services/metrics_plotter_stdout.h"
#include "du_high_config.h"
#include "du_high_config_translators.h"
#include "srsran/du/du_high_wrapper_factory.h"

using namespace srsran;

void srsran::configure_du_high_metrics(const du_high_unit_config&   du_high_unit_cfg,
                                       metrics_plotter_stdout&      metrics_stdout,
                                       metrics_log_helper&          metrics_logger,
                                       metrics_plotter_json&        metrics_json,
                                       rlc_metrics_notifier&        rlc_json_metrics,
                                       e2_metric_connector_manager& e2_metric_connectors,
                                       metrics_hub&                 metrics_hub)
{
  // Generate DU cells.
  auto cells = generate_du_cell_config(du_high_unit_cfg);

  for (const auto& cell : cells) {
    fmt::print("Cell pci={}, bw={} MHz, {}T{}R, dl_arfcn={} (n{}), dl_freq={} MHz, dl_ssb_arfcn={}, ul_freq={} MHz\n",
               cell.pci,
               cell.dl_carrier.carrier_bw_mhz,
               cell.dl_carrier.nof_ant,
               cell.ul_carrier.nof_ant,
               cell.dl_carrier.arfcn,
               srsran::nr_band_to_uint(cell.dl_carrier.band),
               srsran::band_helper::nr_arfcn_to_freq(cell.dl_carrier.arfcn) / 1e6,
               cell.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
               srsran::band_helper::nr_arfcn_to_freq(cell.ul_carrier.arfcn) / 1e6);
  }
  fmt::print("\n");

  // Set up sources for the DU Scheruler UE metrics and add them to metric hub.
  for (unsigned i = 0; i != cells.size(); i++) {
    std::string source_name = "DU " + std::to_string(i);
    auto        source      = std::make_unique<scheduler_ue_metrics_source>(source_name);

    // Connect Console Aggregator to DU Scheduler UE metrics.
    source->add_subscriber(metrics_stdout);

    if (metrics_logger.is_enabled()) {
      source->add_subscriber(metrics_logger);
    }

    // Connect JSON metrics reporter to DU Scheduler UE metrics.
    if (du_high_unit_cfg.metrics.enable_json_metrics) {
      source->add_subscriber(metrics_json);
    }

    // Connect E2 agent to DU Scheduler UE metrics.
    if (du_high_unit_cfg.e2_cfg.enable_du_e2) {
      source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
    }

    metrics_hub.add_source(std::move(source));
  }

  // Set up sources for the DU Scheruler UE metrics and add them to metric hub.
  for (unsigned i = 0; i != cells.size(); i++) {
    if (du_high_unit_cfg.metrics.rlc.report_period != 0) {
      // This source aggregates the RLC metrics from all DRBs in a single DU.
      std::string source_name = "DU RLC " + std::to_string(i);
      auto        rlc_source  = std::make_unique<rlc_metrics_source>(source_name);

      if (du_high_unit_cfg.metrics.rlc.json_enabled) {
        // Connect JSON metrics plotter to RLC metric source.
        rlc_source->add_subscriber(rlc_json_metrics);
      }
      if (metrics_logger.is_enabled()) {
        rlc_source->add_subscriber(metrics_logger);
      }
      if (du_high_unit_cfg.e2_cfg.enable_du_e2) {
        // Connect E2 agent to RLC metric source.
        rlc_source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
      }
      // Pass RLC metric source to the DU high.
      metrics_hub.add_source(std::move(rlc_source));
    }
  }
}

void srsran::fill_du_high_wrapper_config(du_high_wrapper_config&        out_cfg,
                                         const du_high_unit_config&     du_high_unit_cfg,
                                         unsigned                       du_idx,
                                         du_high_executor_mapper&       execution_mapper,
                                         srs_du::f1c_connection_client& f1c_client_handler,
                                         srs_du::f1u_du_gateway&        f1u_gw,
                                         timer_manager&                 timer_mng,
                                         mac_pcap&                      mac_p,
                                         rlc_pcap&                      rlc_p,
                                         e2_connection_client&          e2_client_handler,
                                         e2_metric_connector_manager&   e2_metric_connectors,
                                         metrics_hub&                   metrics_hub)
{
  // DU-high configuration.
  srs_du::du_high_configuration& du_hi_cfg = out_cfg.du_hi;
  du_hi_cfg.cells                          = generate_du_cell_config(du_high_unit_cfg);
  du_hi_cfg.exec_mapper                    = &execution_mapper;
  du_hi_cfg.f1c_client                     = &f1c_client_handler;
  du_hi_cfg.f1u_gw                         = &f1u_gw;
  du_hi_cfg.phy_adapter                    = nullptr;
  du_hi_cfg.timers                         = &timer_mng;
  du_hi_cfg.srbs                           = generate_du_srb_config(du_high_unit_cfg);
  du_hi_cfg.qos                            = generate_du_qos_config(du_high_unit_cfg);
  du_hi_cfg.mac_p                          = &mac_p;
  du_hi_cfg.rlc_p                          = &rlc_p;
  du_hi_cfg.gnb_du_id                      = (gnb_du_id_t)((unsigned)du_high_unit_cfg.gnb_du_id + du_idx);
  du_hi_cfg.gnb_du_name                    = fmt::format("srsdu{}", du_hi_cfg.gnb_du_id);
  du_hi_cfg.mac_cfg                        = generate_mac_expert_config(du_high_unit_cfg);
  // Assign different initial C-RNTIs to different DUs.
  du_hi_cfg.mac_cfg.initial_crnti     = to_rnti(0x4601 + (0x1000 * du_idx));
  du_hi_cfg.sched_ue_metrics_notifier = metrics_hub.get_scheduler_ue_metrics_source("DU " + std::to_string(du_idx));
  du_hi_cfg.rlc_metrics_notif         = metrics_hub.get_rlc_metrics_source("DU RLC " + std::to_string(du_idx));
  du_hi_cfg.sched_cfg                 = generate_scheduler_expert_config(du_high_unit_cfg);

  if (du_high_unit_cfg.e2_cfg.enable_du_e2) {
    // Connect E2 agent to RLC metric source.
    du_hi_cfg.e2_client          = &e2_client_handler;
    du_hi_cfg.e2ap_config        = generate_e2_config(du_high_unit_cfg);
    du_hi_cfg.e2_du_metric_iface = &(e2_metric_connectors.get_e2_du_metrics_interface(du_idx));
  }

  // Configure test mode
  if (du_high_unit_cfg.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
    du_hi_cfg.test_cfg.test_ue =
        srs_du::du_test_config::test_ue_config{du_high_unit_cfg.test_mode_cfg.test_ue.rnti,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.nof_ues,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.auto_ack_indication_delay,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.pdsch_active,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.pusch_active,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.cqi,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.ri,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.pmi,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.i_1_1,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.i_1_3,
                                               du_high_unit_cfg.test_mode_cfg.test_ue.i_2};
  }
}
