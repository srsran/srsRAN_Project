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

#include "split6_o_du_low_unit_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_cli11_schema.h"
#include "split6_o_du_low_unit_config.h"

using namespace srsran;

static ru_ofh_unit_parsed_config ofh_cfg;
static ru_sdr_unit_config        sdr_cfg;

void srsran::configure_cli11_with_split6_o_du_low_unit_config_schema(CLI::App& app, split6_o_du_low_unit_config& config)
{
  configure_cli11_with_du_low_config_schema(app, config.du_low_cfg);
  configure_cli11_with_ru_ofh_config_schema(app, ofh_cfg);
  configure_cli11_with_ru_sdr_config_schema(app, sdr_cfg);
}

static void manage_ru(CLI::App& app, split6_o_du_low_unit_config& config)
{
  // Manage the RU optionals
  auto     ofh_subcmd      = app.get_subcommand("ru_ofh");
  auto     sdr_subcmd      = app.get_subcommand("ru_sdr");
  unsigned nof_ofh_entries = ofh_subcmd->count_all();
  unsigned nof_sdr_entries = sdr_subcmd->count_all();

  // Count the number of RU types.
  unsigned nof_ru_types = (nof_ofh_entries != 0) ? 1 : 0;
  nof_ru_types += (nof_sdr_entries != 0) ? 1 : 0;

  if (nof_ru_types > 1) {
    srsran_terminate(
        "Radio Unit configuration allows either a SDR or Open Fronthaul but not both types at the same time");
  }

  if (nof_ofh_entries != 0) {
    config.ru_cfg = ofh_cfg;
    sdr_subcmd->disabled();

    return;
  }

  config.ru_cfg = sdr_cfg;
  ofh_subcmd->disabled();
}

void srsran::autoderive_split6_o_du_low_parameters_after_parsing(CLI::App& app, split6_o_du_low_unit_config& config)
{
  const unsigned nof_cells = 1U;

  // Auto derive SDR parameters.
  autoderive_ru_sdr_parameters_after_parsing(app, sdr_cfg, nof_cells);
  // Auto derive OFH parameters.
  autoderive_ru_ofh_parameters_after_parsing(app, ofh_cfg);

  // Set the parsed RU.
  manage_ru(app, config);

  // Auto derive DU low parameters.
  const auto&   cell = config.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell;
  const nr_band band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_f_ref_arfcn);
  const bool    is_zmq_rf_driver = false;
  autoderive_du_low_parameters_after_parsing(
      app, config.du_low_cfg, band_helper::get_duplex_mode(band), is_zmq_rf_driver, nof_cells);
}
