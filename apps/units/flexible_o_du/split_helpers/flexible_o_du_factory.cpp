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

#include "flexible_o_du_factory.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_factory.h"
#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "commands/ntn_config_update_remote_command_factory.h"
#include "flexible_o_du_impl.h"
#include "metrics/flexible_o_du_metrics_builder.h"
#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/o_du_factory.h"
#include "srsran/e2/e2_du_metrics_connector.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"

using namespace srsran;

static fapi::prach_config generate_prach_config_tlv(const srs_du::du_cell_config& cell_cfg)
{
  fapi::prach_config config     = {};
  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::kHz1_25;
  config.prach_ul_bwp_pusch_scs = cell_cfg.scs_common;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm;
  config.prach_config_index =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index;
  config.prach_format           = prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.emplace_back();
  fd_occasion.prach_root_sequence_index =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index;
  fd_occasion.prach_freq_offset =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start;
  fd_occasion.prach_zero_corr_conf =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config;

  return config;
}

static fapi::carrier_config generate_carrier_config_tlv(const srs_du::du_cell_config& du_cell)
{
  // Deduce common numerology and grid size for DL and UL.
  unsigned numerology = to_numerology_value(du_cell.scs_common);
  unsigned grid_size_bw_prb =
      band_helper::get_n_rbs_from_bw(MHz_to_bs_channel_bandwidth(du_cell.dl_carrier.carrier_bw_mhz),
                                     du_cell.scs_common,
                                     band_helper::get_freq_range(du_cell.dl_carrier.band));

  fapi::carrier_config fapi_config = {};

  // NOTE; for now we only need to fill the nof_prb_ul_grid and nof_prb_dl_grid for the common SCS.
  fapi_config.dl_grid_size             = {};
  fapi_config.dl_grid_size[numerology] = grid_size_bw_prb;
  fapi_config.ul_grid_size             = {};
  fapi_config.ul_grid_size[numerology] = grid_size_bw_prb;

  // Number of transmit and receive antenna ports.
  fapi_config.num_tx_ant = du_cell.dl_carrier.nof_ant;
  fapi_config.num_rx_ant = du_cell.ul_carrier.nof_ant;

  return fapi_config;
}

static o_du_low_unit_config generate_o_du_low_config(const du_low_unit_config&            du_low_unit_cfg,
                                                     float                                dBFS_calibration_value,
                                                     span<const srs_du::du_cell_config>   cells,
                                                     span<const du_high_unit_cell_config> du_hi_cells)
{
  o_du_low_unit_config odu_low_cfg = {du_low_unit_cfg, {}, {}};

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    const auto& cell        = cells[i];
    const auto& du_hi_cell  = du_hi_cells[i];
    auto&       fapi_sector = odu_low_cfg.fapi_cfg.sectors.emplace_back();

    fapi_sector.carrier_cfg                   = generate_carrier_config_tlv(cell);
    fapi_sector.prach_cfg                     = generate_prach_config_tlv(cell);
    fapi_sector.allow_request_on_empty_ul_tti = du_low_unit_cfg.expert_phy_cfg.allow_request_on_empty_uplink_slot;
    fapi_sector.nof_slots_request_headroom    = du_low_unit_cfg.expert_phy_cfg.nof_slots_request_headroom;
    fapi_sector.prach_ports                   = du_hi_cell.cell.prach_cfg.ports;
    fapi_sector.scs                           = cell.scs_common;
    fapi_sector.scs_common                    = cell.scs_common;
    fapi_sector.dBFS_calibration_value        = dBFS_calibration_value;
    fapi_sector.sector_id                     = i;

    auto&   du_low_cell    = odu_low_cfg.cells.emplace_back();
    nr_band band           = cell.dl_carrier.band;
    du_low_cell.duplex     = band_helper::get_duplex_mode(band);
    du_low_cell.freq_range = band_helper::get_freq_range(band);
    du_low_cell.bw_rb      = band_helper::get_n_rbs_from_bw(
        MHz_to_bs_channel_bandwidth(cell.dl_carrier.carrier_bw_mhz), cell.scs_common, du_low_cell.freq_range);
    du_low_cell.nof_rx_antennas = cell.ul_carrier.nof_ant;
    du_low_cell.nof_tx_antennas = cell.dl_carrier.nof_ant;
    du_low_cell.prach_ports     = du_hi_cell.cell.prach_cfg.ports;
    du_low_cell.scs_common      = cell.scs_common;
    du_low_cell.prach_config_index =
        cell.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index;
    du_low_cell.max_puschs_per_slot  = du_hi_cell.cell.pusch_cfg.max_puschs_per_slot;
    du_low_cell.pusch_max_nof_layers = cell.pusch_max_nof_layers;
    du_low_cell.tdd_pattern          = cell.tdd_ul_dl_cfg_common;
  }

  return odu_low_cfg;
}

static flexible_o_du_ru_config generate_o_du_ru_config(span<const srs_du::du_cell_config> cells,
                                                       unsigned                           max_processing_delay,
                                                       unsigned                           prach_nof_ports)
{
  flexible_o_du_ru_config out_cfg;
  out_cfg.prach_nof_ports      = prach_nof_ports;
  out_cfg.max_processing_delay = max_processing_delay;

  for (const auto& cell : cells) {
    auto& out_cell           = out_cfg.cells.emplace_back();
    out_cell.nof_tx_antennas = cell.dl_carrier.nof_ant;
    out_cell.nof_rx_antennas = cell.ul_carrier.nof_ant;
    out_cell.scs             = cell.scs_common;
    out_cell.dl_arfcn        = cell.dl_carrier.arfcn_f_ref;
    out_cell.ul_arfcn        = cell.ul_carrier.arfcn_f_ref;
    out_cell.tdd_config      = cell.tdd_ul_dl_cfg_common;
    out_cell.bw              = MHz_to_bs_channel_bandwidth(cell.dl_carrier.carrier_bw_mhz);
    out_cell.freq_range      = band_helper::get_freq_range(cell.dl_carrier.band);
    out_cell.cp              = cell.dl_cfg_common.init_dl_bwp.generic_params.cp;
  }

  return out_cfg;
}

o_du_unit flexible_o_du_factory::create_flexible_o_du(const o_du_unit_dependencies& dependencies)
{
  o_du_unit      o_du;
  const unsigned nof_cells  = config.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  o_du.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(nof_cells);

  const du_high_unit_config& du_hi = config.odu_high_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo = config.du_low_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<pci_t> pci_cell_mapper;

  for (const auto& cell : du_cells) {
    pci_cell_mapper.push_back(cell.pci);
  }

  std::chrono::nanoseconds symbol_duration(
      static_cast<int64_t>(1e6 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) *
                                  get_nof_slots_per_subframe(du_hi.cells_cfg.front().cell.common_scs))));

  std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>
      ru_metrics_subcommands;
  // Create flexible O-DU metrics configuration.
  flexible_o_du_metrics_notifier* flexible_odu_metrics_notifier =
      build_flexible_o_du_metrics_config(o_du.metrics,
                                         ru_metrics_subcommands,
                                         *dependencies.metrics_notifier,
                                         config.ru_cfg.config,
                                         std::move(pci_cell_mapper),
                                         symbol_duration);

  // Create flexible O-DU implementation.
  auto du_impl = std::make_unique<flexible_o_du_impl>(nof_cells, flexible_odu_metrics_notifier);

  o_du_low_unit_config odu_low_cfg =
      generate_o_du_low_config(du_lo, config.ru_cfg.dBFS_calibration_value, du_cells, du_hi.cells_cfg);
  o_du_low_unit_dependencies odu_low_dependencies = {.rg_gateway = du_impl->get_upper_ru_dl_rg_adapter(),
                                                     .rx_symbol_request_notifier =
                                                         du_impl->get_upper_ru_ul_request_adapter(),
                                                     .workers = dependencies.workers->get_du_low_executor_mapper()};
  o_du_low_unit_factory      odu_low_factory(du_lo.hal_config);
  auto                       odu_lo_unit = odu_low_factory.create(odu_low_cfg, odu_low_dependencies);

  std::for_each(odu_lo_unit.metrics.begin(), odu_lo_unit.metrics.end(), [&](auto& e) {
    o_du.metrics.emplace_back(std::move(e));
  });

  o_du_high_unit_dependencies odu_hi_unit_dependencies = {dependencies.workers->get_du_high_executor_mapper(),
                                                          *dependencies.f1c_client_handler,
                                                          *dependencies.f1u_gw,
                                                          *dependencies.timer_ctrl,
                                                          *dependencies.mac_p,
                                                          *dependencies.rlc_p,
                                                          *dependencies.e2_client_handler,
                                                          *(o_du.e2_metric_connectors),
                                                          *dependencies.metrics_notifier,
                                                          {}};

  // Adjust the dependencies.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    auto& sector_dependencies = odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.emplace_back();
    sector_dependencies.gateway =
        &odu_lo_unit.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(i).get_slot_message_gateway();
    sector_dependencies.last_msg_notifier =
        &odu_lo_unit.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(i).get_slot_last_message_notifier();
    sector_dependencies.fapi_executor = config.odu_high_cfg.fapi_cfg.l2_nof_slots_ahead != 0
                                            ? std::optional(dependencies.workers->fapi_exec[i])
                                            : std::make_optional<task_executor*>();
  }

  o_du_high_unit odu_hi_unit = make_o_du_high_unit(config.odu_high_cfg, std::move(odu_hi_unit_dependencies));

  // Connect the adaptors.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    fapi_adaptor::phy_fapi_sector_adaptor& odu_lo = odu_lo_unit.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(i);
    fapi_adaptor::mac_fapi_sector_adaptor& odu_hi = odu_hi_unit.o_du_hi->get_mac_fapi_adaptor().get_sector_adaptor(i);

    // Connect O-DU low with O-DU high.
    odu_lo.set_slot_time_message_notifier(odu_hi.get_slot_time_message_notifier());
    odu_lo.set_error_message_notifier(odu_hi.get_error_message_notifier());
    odu_lo.set_slot_data_message_notifier(odu_hi.get_slot_data_message_notifier());
  }

  std::for_each(odu_hi_unit.metrics.begin(), odu_hi_unit.metrics.end(), [&](auto& e) {
    o_du.metrics.emplace_back(std::move(e));
  });

  // Manage commands.
  o_du.commands = std::move(odu_hi_unit.commands);

  srs_du::o_du_dependencies odu_dependencies;
  odu_dependencies.odu_hi           = std::move(odu_hi_unit.o_du_hi);
  odu_dependencies.odu_lo           = std::move(odu_lo_unit.o_du_lo);
  odu_dependencies.metrics_notifier = &du_impl->get_o_du_metrics_notifier();

  auto odu_instance = make_o_du(std::move(odu_dependencies));
  report_error_if_not(odu_instance, "Invalid Distributed Unit");

  flexible_o_du_ru_config ru_config = generate_o_du_ru_config(
      du_cells, du_lo.expert_phy_cfg.max_processing_delay_slots, du_hi.cells_cfg.front().cell.prach_cfg.ports.size());
  flexible_o_du_ru_dependencies ru_dependencies{*dependencies.workers,
                                                du_impl->get_upper_ru_ul_adapter(),
                                                du_impl->get_upper_ru_timing_adapter(),
                                                du_impl->get_upper_ru_error_adapter()};

  std::unique_ptr<radio_unit> ru = create_radio_unit(ru_config, ru_dependencies);

  srsran_assert(ru, "Invalid Radio Unit");

  // Add RU metrics subcommands.
  for (auto& subcmd : ru_metrics_subcommands) {
    o_du.commands.cmdline.metrics_subcommands.emplace_back(std::move(subcmd));
  }

  // Add RU command-line commands.
  o_du.commands.cmdline.commands.push_back(std::make_unique<change_log_level_app_command>());

  // Create the RU gain commands.
  if (auto* controller = ru->get_controller().get_gain_controller()) {
    o_du.commands.cmdline.commands.push_back(std::make_unique<tx_gain_app_command>(*controller));
    o_du.commands.cmdline.commands.push_back(std::make_unique<rx_gain_app_command>(*controller));
  }

  // Create the RU CFO command.
  if (auto* controller = ru->get_controller().get_cfo_controller()) {
    o_du.commands.cmdline.commands.push_back(std::make_unique<cfo_app_command>(*controller));
  }

  // Create the RU transmit time offset command.
  if (auto* controller = ru->get_controller().get_tx_time_offset_controller()) {
    o_du.commands.cmdline.commands.push_back(std::make_unique<tx_time_offset_app_command>(*controller));
  }

  // Add NTN-Config update remote command.
  auto& ntn_cfg = config.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell.ntn_cfg;
  if (ntn_cfg.has_value()) {
    add_ntn_config_update_remote_command(
        o_du.commands,
        ntn_cfg.value(),
        odu_instance->get_o_du_high().get_du_high().get_du_configurator(),
        odu_instance->get_o_du_high().get_du_high().get_du_manager_time_mapper_accessor(),
        ru->get_controller(),
        dependencies.timer_ctrl->get_timer_manager(),
        dependencies.workers->get_du_high_executor_mapper().du_control_executor());
  }

  // Configure the RU and DU in the dynamic DU.
  du_impl->add_ru(std::move(ru));
  du_impl->add_du(std::move(odu_instance));

  o_du.unit = std::move(du_impl);

  announce_du_high_cells(du_hi);

  return o_du;
}
