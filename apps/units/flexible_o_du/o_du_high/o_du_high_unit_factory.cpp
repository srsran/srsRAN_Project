/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "o_du_high_unit_factory.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "du_high/commands/du_high_cmdline_commands.h"
#include "du_high/commands/du_high_remote_commands.h"
#include "du_high/du_high_config_translators.h"
#include "du_high/metrics/du_high_rlc_metrics.h"
#include "du_high/metrics/du_high_rlc_metrics_consumers.h"
#include "du_high/metrics/du_high_rlc_metrics_producer.h"
#include "du_high/metrics/du_metrics.h"
#include "du_high/metrics/du_metrics_consumers.h"
#include "du_high/metrics/du_metrics_producer.h"
#include "e2/o_du_high_e2_config_translators.h"
#include "o_du_high_unit_config.h"
#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/du_high_configuration.h"
#include "srsran/du/du_high/o_du_high_config.h"
#include "srsran/du/du_high/o_du_high_factory.h"
#include "srsran/du/du_update_config_helpers.h"
#include "srsran/e2/e2_du_metrics_connector.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"

using namespace srsran;

void srsran::announce_du_high_cells(const du_high_unit_config& du_high_unit_cfg)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB", false);

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

    logger.info(
        "SSB derived parameters for cell: {}, band: {}, dl_arfcn:{}, nof_crbs: {} scs:{}, ssb_scs:{}:\n\t - SSB offset "
        "pointA:{} \n\t - k_SSB:{} \n\t - SSB arfcn:{} \n\t - Coreset index:{} \n\t - Searchspace index:{}",
        cell.pci,
        fmt::underlying(cell.dl_carrier.band),
        cell.dl_carrier.arfcn_f_ref,
        cell.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(),
        to_string(cell.dl_cfg_common.init_dl_bwp.generic_params.scs),
        to_string(cell.ssb_cfg.scs),
        cell.ssb_cfg.offset_to_point_A.to_uint(),
        cell.ssb_cfg.k_ssb.to_uint(),
        cell.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
        cell.coreset0_idx,
        cell.searchspace0_idx);
  }

  fmt::print("\n");
}

static void validates_derived_du_params(span<const srs_du::du_cell_config> cells)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("DU", false);

  for (const auto& cell_cfg : cells) {
    const rach_config_common& rach_cfg = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

    frequency_range freq_range = band_helper::get_freq_range(cell_cfg.dl_carrier.band);
    duplex_mode     dplx_mode  = band_helper::get_duplex_mode(cell_cfg.dl_carrier.band);

    const auto prach_cfg = prach_configuration_get(freq_range, dplx_mode, rach_cfg.rach_cfg_generic.prach_config_index);

    prb_interval prb_interval_no_pucch = config_helpers::find_largest_prb_interval_without_pucch(
        cell_cfg.pucch_cfg, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

    // This is to preserve a guardband between the PUCCH and PRACH.
    const unsigned pucch_to_prach_guardband = is_long_preamble(prach_cfg.format) ? 0U : 3U;

    if (rach_cfg.rach_cfg_generic.msg1_frequency_start < prb_interval_no_pucch.start() + pucch_to_prach_guardband) {
      fmt::print("Warning: With the given prach_frequency_start={}, the PRACH opportunities overlap with the PUCCH "
                 "resources/guardband in prbs=[0, {}). Some interference between PUCCH and PRACH should be "
                 "expected\n",
                 rach_cfg.rach_cfg_generic.msg1_frequency_start,
                 prb_interval_no_pucch.start() + pucch_to_prach_guardband);

      logger.warning(
          "With the given prach_frequency_start={}, the PRACH opportunities overlap with the PUCCH resources/guardband "
          "in prbs=[0, {}). Some interference between PUCCH and PRACH should be expected",
          rach_cfg.rach_cfg_generic.msg1_frequency_start,
          prb_interval_no_pucch.start() + pucch_to_prach_guardband);
    }
  }
}

static rlc_metrics_notifier* build_rlc_du_metrics(std::vector<app_services::metrics_config>& metrics,
                                                  app_services::metrics_notifier&            metrics_notifier,
                                                  const o_du_high_unit_config&               o_du_high_unit_cfg,
                                                  e2_du_metrics_notifier&                    e2_notifier)
{
  rlc_metrics_notifier*      out       = nullptr;
  const du_high_unit_config& du_hi_cfg = o_du_high_unit_cfg.du_high_cfg.config;

  // RLC metrics not enabled, do not add metrics configuration.
  if (!du_hi_cfg.metrics.layers_cfg.enable_rlc) {
    return out;
  }

  app_services::metrics_config& rlc_metrics_cfg = metrics.emplace_back();
  rlc_metrics_cfg.metric_name                   = rlc_metrics_properties_impl().name();
  rlc_metrics_cfg.callback                      = rlc_metrics_callback;

  // Fill the generator.
  auto rlc_metric_gen = std::make_unique<rlc_metrics_producer_impl>(metrics_notifier);
  out                 = &(*rlc_metric_gen);
  rlc_metrics_cfg.producers.push_back(std::move(rlc_metric_gen));

  const app_helpers::metrics_config& metrics_config = du_hi_cfg.metrics.common_metrics_cfg;
  // Consumers.
  if (metrics_config.enable_log_metrics) {
    rlc_metrics_cfg.consumers.push_back(
        std::make_unique<rlc_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_config.enable_json_metrics) {
    rlc_metrics_cfg.consumers.push_back(
        std::make_unique<rlc_metrics_consumer_json>(app_helpers::fetch_json_metrics_log_channel()));
  }

  if (o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2) {
    rlc_metrics_cfg.consumers.push_back(std::make_unique<rlc_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

static srs_du::du_metrics_notifier*
build_du_metrics(std::vector<app_services::metrics_config>& metrics,
                 std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>&
                                                 metrics_subcommands,
                 app_services::metrics_notifier& metrics_notifier,
                 const o_du_high_unit_config&    o_du_high_unit_cfg,
                 e2_du_metrics_notifier&         e2_notifier)
{
  const du_high_unit_config& du_hi_cfg = o_du_high_unit_cfg.du_high_cfg.config;

  // Scheduler or MAC metrics not enabled, do not create consumers and producers.
  if (!du_hi_cfg.metrics.layers_cfg.enable_mac && !du_hi_cfg.metrics.layers_cfg.enable_scheduler) {
    return nullptr;
  }

  srs_du::du_metrics_notifier* out = nullptr;

  app_services::metrics_config& du_metrics_cfg = metrics.emplace_back();
  du_metrics_cfg.metric_name                   = du_metrics_properties_impl().name();
  du_metrics_cfg.callback                      = du_metrics_callback;

  // Fill the generator.
  auto du_metric_gen = std::make_unique<du_metrics_producer_impl>(metrics_notifier);
  out                = &(*du_metric_gen);
  du_metrics_cfg.producers.push_back(std::move(du_metric_gen));

  // Create the consumer for STDOUT. Also create the command for toggle the metrics.
  auto metrics_stdout = std::make_unique<du_metrics_consumer_stdout>();
  metrics_subcommands.push_back(std::make_unique<du_high_metrics_subcommand_stdout>(*metrics_stdout));
  du_metrics_cfg.consumers.push_back(std::move(metrics_stdout));

  const app_helpers::metrics_config& metrics_config = du_hi_cfg.metrics.common_metrics_cfg;
  if (metrics_config.enable_log_metrics) {
    du_metrics_cfg.consumers.push_back(
        std::make_unique<du_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_config.enable_json_metrics) {
    du_metrics_cfg.consumers.push_back(
        std::make_unique<du_metrics_consumer_json>(app_helpers::fetch_json_metrics_log_channel()));
  }

  // Connect E2 agent to DU Scheduler UE metrics.
  if (o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2) {
    du_metrics_cfg.consumers.push_back(std::make_unique<du_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

o_du_high_unit srsran::make_o_du_high_unit(const o_du_high_unit_config&  o_du_high_unit_cfg,
                                           o_du_high_unit_dependencies&& dependencies)
{
  srs_du::o_du_high_config       o_du_high_cfg;
  srs_du::du_high_configuration& du_hi_cfg        = o_du_high_cfg.du_hi;
  const du_high_unit_config&     du_high_unit_cfg = o_du_high_unit_cfg.du_high_cfg.config;

  // Generate DU high config from the unit config.
  generate_du_high_config(du_hi_cfg, du_high_unit_cfg);

  // Validates the derived parameters.
  validates_derived_du_params(du_hi_cfg.ran.cells);

  srs_du::du_high_dependencies& du_hi_deps = dependencies.o_du_hi_dependencies.du_hi;
  du_hi_deps.exec_mapper                   = &dependencies.execution_mapper;
  du_hi_deps.f1c_client                    = &dependencies.f1c_client_handler;
  du_hi_deps.f1u_gw                        = &dependencies.f1u_gw;
  du_hi_deps.phy_adapter                   = nullptr;
  du_hi_deps.timer_ctrl                    = &dependencies.timer_ctrl;
  du_hi_deps.mac_p                         = &dependencies.mac_p;
  du_hi_deps.rlc_p                         = &dependencies.rlc_p;

  if (o_du_high_unit_cfg.e2_cfg.base_cfg.enable_unit_e2) {
    // Connect E2 agent to RLC metric source.
    dependencies.o_du_hi_dependencies.e2_client = &dependencies.e2_client_handler;
    o_du_high_cfg.e2ap_config                   = generate_e2_config(o_du_high_unit_cfg.e2_cfg,
                                                   du_high_unit_cfg.gnb_id,
                                                   du_high_unit_cfg.cells_cfg.front().cell.plmn,
                                                   du_hi_cfg.ran.gnb_du_id);
    dependencies.o_du_hi_dependencies.e2_du_metric_iface =
        &(dependencies.e2_metric_connectors.get_e2_metrics_interface(0));
  }

  // DU high metrics.
  o_du_high_unit odu_unit;

  du_hi_deps.du_notifier = build_du_metrics(odu_unit.metrics,
                                            odu_unit.commands.cmdline.metrics_subcommands,
                                            dependencies.metrics_notifier,
                                            o_du_high_unit_cfg,
                                            dependencies.e2_metric_connectors.get_e2_metric_notifier(0));

  du_hi_deps.rlc_metrics_notif = build_rlc_du_metrics(odu_unit.metrics,
                                                      dependencies.metrics_notifier,
                                                      o_du_high_unit_cfg,
                                                      dependencies.e2_metric_connectors.get_e2_metric_notifier(0));

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

  // FAPI configuration.
  const fapi_unit_config& fapi_cfg      = o_du_high_unit_cfg.fapi_cfg;
  o_du_high_cfg.fapi.log_level          = fapi_cfg.fapi_level;
  o_du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;

  // Create O-DU high.
  odu_unit.o_du_hi = srs_du::make_o_du_high(o_du_high_cfg, std::move(dependencies.o_du_hi_dependencies));
  report_error_if_not(odu_unit.o_du_hi, "Invalid O-DU high");

  // Create remote commands.
  odu_unit.commands.remote.push_back(
      std::make_unique<ssb_modify_remote_command>(odu_unit.o_du_hi->get_du_high().get_du_configurator()));
  odu_unit.commands.remote.push_back(
      std::make_unique<rrm_policy_ratio_remote_command>(odu_unit.o_du_hi->get_du_high().get_du_configurator()));

  return odu_unit;
}
