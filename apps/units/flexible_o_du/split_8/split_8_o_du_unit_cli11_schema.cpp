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

#include "split_8_o_du_unit_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_cli11_schema.h"
#include "split_8_o_du_unit_config.h"

using namespace srsran;

void srsran::configure_cli11_with_split_8_o_du_unit_config_schema(CLI::App& app, split_8_o_du_unit_config& parsed_cfg)
{
  configure_cli11_with_o_du_high_config_schema(app, parsed_cfg.odu_high_cfg);
  configure_cli11_with_du_low_config_schema(app, parsed_cfg.du_low_cfg);
  configure_cli11_with_ru_sdr_config_schema(app, parsed_cfg.ru_cfg);
}

void srsran::autoderive_split_8_o_du_parameters_after_parsing(CLI::App& app, split_8_o_du_unit_config& parsed_cfg)
{
  const unsigned nof_cells = parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  autoderive_o_du_high_parameters_after_parsing(app, parsed_cfg.odu_high_cfg);
  // Auto derive SDR parameters.
  autoderive_ru_sdr_parameters_after_parsing(app, parsed_cfg.ru_cfg, nof_cells);

  // Auto derive DU low parameters.
  const auto& cell = parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell;
  nr_band     band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_f_ref_arfcn);

  autoderive_du_low_parameters_after_parsing(app, parsed_cfg.du_low_cfg, band_helper::get_duplex_mode(band));
}
