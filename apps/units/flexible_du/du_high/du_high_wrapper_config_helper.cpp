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
#include "du_high_commands.h"
#include "du_high_config.h"
#include "du_high_config_translators.h"
#include "metrics/du_high_rlc_metrics_consumers.h"
#include "metrics/du_high_rlc_metrics_producer.h"
#include "metrics/du_high_scheduler_cell_metrics_consumers.h"
#include "metrics/du_high_scheduler_cell_metrics_producer.h"
#include "srsran/du/du_high/du_high_wrapper_factory.h"

using namespace srsran;

void srsran::announce_du_high_cells(const du_high_unit_config& du_high_unit_cfg)
{
  // Generate DU cells.
  auto cells = generate_du_cell_config(du_high_unit_cfg);

  for (const auto& cell : cells) {
    fmt::print("Cell pci={}, bw={} MHz, {}T{}R, dl_arfcn={} (n{}), dl_freq={} MHz, dl_ssb_arfcn={}, ul_freq={} MHz\n",
               cell.pci,
               cell.dl_carrier.carrier_bw_mhz,
               cell.dl_carrier.nof_ant,
               cell.ul_carrier.nof_ant,
               cell.dl_carrier.arfcn_f_ref,
               srsran::nr_band_to_uint(cell.dl_carrier.band),
               srsran::band_helper::nr_arfcn_to_freq(cell.dl_carrier.arfcn_f_ref) / 1e6,
               cell.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
               srsran::band_helper::nr_arfcn_to_freq(cell.ul_carrier.arfcn_f_ref) / 1e6);
  }

  fmt::print("\n");
}

static scheduler_metrics_notifier*
build_scheduler_du_metrics(std::pair<std::vector<app_services::metrics_config>,
                                     std::vector<std::unique_ptr<app_services::application_command>>>& du_services_cfg,
                           app_services::metrics_notifier&                                             metrics_notifier,
                           const du_high_unit_config&                                                  du_high_unit_cfg,
                           srslog::sink&                                                               json_sink,
                           e2_du_metrics_notifier&                                                     e2_notifier)
{
  // Scheduler cell metrics.
  auto sched_cell_metrics_gen                     = std::make_unique<scheduler_metrics_producer_impl>(metrics_notifier);
  scheduler_metrics_notifier*   out               = &(*sched_cell_metrics_gen);
  app_services::metrics_config& sched_metrics_cfg = du_services_cfg.first.emplace_back();
  sched_metrics_cfg.metric_name                   = scheduler_cell_metrics_property_impl().name();
  sched_metrics_cfg.callback                      = sched_cell_metrics_gen->get_callback();
  sched_metrics_cfg.producers.push_back(std::move(sched_cell_metrics_gen));

  // Create the consumer for STDOUT. Also create the command for toogle the metrics.
  auto metrics_stdout =
      std::make_unique<scheduler_cell_metrics_consumer_stdout>(du_high_unit_cfg.metrics.autostart_stdout_metrics);
  du_services_cfg.second.push_back(std::make_unique<toggle_stdout_metrics_app_command>(*metrics_stdout));
  sched_metrics_cfg.consumers.push_back(std::move(metrics_stdout));

  if (du_high_unit_cfg.loggers.metrics_level == srslog::basic_levels::info ||
      du_high_unit_cfg.loggers.metrics_level == srslog::basic_levels::debug) {
    sched_metrics_cfg.consumers.push_back(
        std::make_unique<scheduler_cell_metrics_consumer_log>(srslog::fetch_basic_logger("METRICS")));
  }

  // Connect JSON metrics reporter to DU Scheduler UE metrics.
  if (du_high_unit_cfg.metrics.enable_json_metrics) {
    srslog::log_channel& json_channel = srslog::fetch_log_channel("JSON_channel", json_sink, {});
    json_channel.set_enabled(true);
    sched_metrics_cfg.consumers.push_back(std::make_unique<scheduler_cell_metrics_consumer_json>(json_channel));
  }

  // Connect E2 agent to DU Scheduler UE metrics.
  if (du_high_unit_cfg.e2_cfg.enable_du_e2) {
    sched_metrics_cfg.consumers.push_back(std::make_unique<scheduler_cell_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

static rlc_metrics_notifier* build_rlc_du_metrics(std::vector<app_services::metrics_config>& metrics,
                                                  app_services::metrics_notifier&            metrics_notifier,
                                                  const du_high_unit_config&                 du_high_unit_cfg,
                                                  srslog::sink&                              json_sink,
                                                  e2_du_metrics_notifier&                    e2_notifier)
{
  rlc_metrics_notifier* out = nullptr;

  // RLC metrics.
  if (!du_high_unit_cfg.metrics.rlc.json_enabled && !du_high_unit_cfg.e2_cfg.enable_du_e2 &&
      du_high_unit_cfg.loggers.metrics_level != srslog::basic_levels::debug) {
    return out;
  }

  app_services::metrics_config& rlc_metrics_cfg = metrics.emplace_back();
  rlc_metrics_cfg.metric_name                   = rlc_metrics_properties_impl().name();
  rlc_metrics_cfg.callback                      = rlc_metrics_callback;

  // Fill the generator.
  auto rlc_metric_gen = std::make_unique<rlc_metrics_producer_impl>(metrics_notifier);
  out                 = &(*rlc_metric_gen);
  rlc_metrics_cfg.producers.push_back(std::move(rlc_metric_gen));

  // Consumers.
  if (du_high_unit_cfg.loggers.metrics_level == srslog::basic_levels::debug) {
    rlc_metrics_cfg.consumers.push_back(
        std::make_unique<rlc_metrics_consumer_log>(srslog::fetch_basic_logger("METRICS")));
  }

  if (du_high_unit_cfg.metrics.rlc.json_enabled) {
    srslog::log_channel& rlc_json_channel = srslog::fetch_log_channel("JSON_RLC_channel", json_sink, {});
    rlc_json_channel.set_enabled(true);
    rlc_metrics_cfg.consumers.push_back(std::make_unique<rlc_metrics_consumer_json>(rlc_json_channel));
  }

  if (du_high_unit_cfg.e2_cfg.enable_du_e2) {
    rlc_metrics_cfg.consumers.push_back(std::make_unique<rlc_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

std::pair<std::vector<app_services::metrics_config>, std::vector<std::unique_ptr<app_services::application_command>>>
srsran::fill_du_high_wrapper_config(srs_du::du_high_wrapper_config&  out_cfg,
                                    const du_high_unit_config&       du_high_unit_cfg,
                                    unsigned                         du_idx,
                                    srs_du::du_high_executor_mapper& execution_mapper,
                                    srs_du::f1c_connection_client&   f1c_client_handler,
                                    srs_du::f1u_du_gateway&          f1u_gw,
                                    timer_manager&                   timer_mng,
                                    mac_pcap&                        mac_p,
                                    rlc_pcap&                        rlc_p,
                                    e2_connection_client&            e2_client_handler,
                                    e2_metric_connector_manager&     e2_metric_connectors,
                                    srslog::sink&                    json_sink,
                                    app_services::metrics_notifier&  metrics_notifier)
{
  // DU-high configuration.
  srs_du::du_high_configuration& du_hi_cfg = out_cfg.du_hi;
  du_hi_cfg.ran.gnb_du_id   = static_cast<gnb_du_id_t>((static_cast<unsigned>(du_high_unit_cfg.gnb_du_id) + du_idx));
  du_hi_cfg.ran.gnb_du_name = fmt::format("srsdu{}", du_hi_cfg.ran.gnb_du_id);
  du_hi_cfg.ran.cells       = generate_du_cell_config(du_high_unit_cfg);
  du_hi_cfg.ran.srbs        = generate_du_srb_config(du_high_unit_cfg);
  du_hi_cfg.ran.qos         = generate_du_qos_config(du_high_unit_cfg);
  du_hi_cfg.ran.mac_cfg     = generate_mac_expert_config(du_high_unit_cfg);
  // Assign different initial C-RNTIs to different DUs.
  du_hi_cfg.ran.mac_cfg.initial_crnti = to_rnti(0x4601 + (0x1000 * du_idx));
  du_hi_cfg.ran.sched_cfg             = generate_scheduler_expert_config(du_high_unit_cfg);
  du_hi_cfg.exec_mapper               = &execution_mapper;
  du_hi_cfg.f1c_client                = &f1c_client_handler;
  du_hi_cfg.f1u_gw                    = &f1u_gw;
  du_hi_cfg.phy_adapter               = nullptr;
  du_hi_cfg.timers                    = &timer_mng;
  du_hi_cfg.mac_p                     = &mac_p;
  du_hi_cfg.rlc_p                     = &rlc_p;

  if (du_high_unit_cfg.e2_cfg.enable_du_e2) {
    // Connect E2 agent to RLC metric source.
    du_hi_cfg.e2_client          = &e2_client_handler;
    du_hi_cfg.e2ap_config        = generate_e2_config(du_high_unit_cfg);
    du_hi_cfg.e2_du_metric_iface = &(e2_metric_connectors.get_e2_du_metrics_interface(du_idx));
  }

  // DU high metrics.
  std::pair<std::vector<app_services::metrics_config>, std::vector<std::unique_ptr<app_services::application_command>>>
      du_services_cfg;
  du_hi_cfg.sched_ue_metrics_notifier =
      build_scheduler_du_metrics(du_services_cfg,
                                 metrics_notifier,
                                 du_high_unit_cfg,
                                 json_sink,
                                 e2_metric_connectors.get_e2_du_metric_notifier(du_idx));

  du_hi_cfg.rlc_metrics_notif = build_rlc_du_metrics(du_services_cfg.first,
                                                     metrics_notifier,
                                                     du_high_unit_cfg,
                                                     json_sink,
                                                     e2_metric_connectors.get_e2_du_metric_notifier(du_idx));

  // Configure test mode
  if (du_high_unit_cfg.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
    du_hi_cfg.test_cfg.test_ue = srs_du::du_test_mode_config::test_mode_ue_config{
        du_high_unit_cfg.test_mode_cfg.test_ue.rnti,
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

  return du_services_cfg;
}
