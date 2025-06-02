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

#include "dynamic_o_du_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_translators.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_translator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_translator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_translator.h"
#include "dynamic_o_du_unit_config.h"
#include "srsran/du/du_cell_config.h"

using namespace srsran;

ru_dummy_configuration srsran::generate_ru_dummy_config(const ru_dummy_unit_config&        ru_cfg,
                                                        span<const srs_du::du_cell_config> du_cells,
                                                        unsigned                           max_processing_delay_slots,
                                                        unsigned                           nof_prach_ports)
{
  ru_dummy_configuration out_cfg;

  const srs_du::du_cell_config& cell = du_cells.front();

  // Derive parameters.
  unsigned channel_bw_prb = band_helper::get_n_rbs_from_bw(
      MHz_to_bs_channel_bandwidth(cell.dl_carrier.carrier_bw_mhz), cell.scs_common, frequency_range::FR1);

  // Fill configuration parameters.
  out_cfg.are_metrics_enabled        = ru_cfg.metrics_cfg.enable_ru_metrics;
  out_cfg.scs                        = cell.scs_common;
  out_cfg.nof_sectors                = du_cells.size();
  out_cfg.rx_rg_nof_prb              = channel_bw_prb;
  out_cfg.rx_rg_nof_ports            = cell.ul_carrier.nof_ant;
  out_cfg.rx_prach_nof_ports         = nof_prach_ports;
  out_cfg.max_processing_delay_slots = max_processing_delay_slots;
  out_cfg.dl_processing_delay        = ru_cfg.dl_processing_delay;
  out_cfg.time_scaling               = ru_cfg.time_scaling;

  return out_cfg;
}

static void fill_dummy_worker_manager_config(worker_manager_config& config, const ru_dummy_unit_config& ru_cfg)
{
  config.ru_dummy_cfg.emplace();

  srsran_assert(config.config_affinities.size() == ru_cfg.cell_affinities.size(), "Invalid number of cell affinities");

  for (unsigned i = 0, e = ru_cfg.cell_affinities.size(); i != e; ++i) {
    config.config_affinities[i].push_back(ru_cfg.cell_affinities[i].ru_cpu_cfg);
  }
}

void srsran::fill_dynamic_du_worker_manager_config(worker_manager_config&          config,
                                                   const dynamic_o_du_unit_config& unit_cfg)
{
  bool is_blocking_mode_enable = false;
  if (std::holds_alternative<ru_sdr_unit_config>(unit_cfg.ru_cfg)) {
    is_blocking_mode_enable = std::get<ru_sdr_unit_config>(unit_cfg.ru_cfg).device_driver == "zmq";
  }
  unsigned nof_cells = unit_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  fill_o_du_high_worker_manager_config(config, unit_cfg.odu_high_cfg, is_blocking_mode_enable);
  fill_du_low_worker_manager_config(config, unit_cfg.du_low_cfg, is_blocking_mode_enable, nof_cells);

  if (auto* ru_sdr = std::get_if<ru_sdr_unit_config>(&unit_cfg.ru_cfg)) {
    fill_sdr_worker_manager_config(config, *ru_sdr);
  } else if (auto* ru_ofh = std::get_if<ru_ofh_unit_parsed_config>(&unit_cfg.ru_cfg)) {
    auto cells = generate_du_cell_config(unit_cfg.odu_high_cfg.du_high_cfg.config);
    fill_ofh_worker_manager_config(config, ru_ofh->config, cells);
  } else if (auto* ru_dummy = std::get_if<ru_dummy_unit_config>(&unit_cfg.ru_cfg)) {
    fill_dummy_worker_manager_config(config, *ru_dummy);
  }
}
