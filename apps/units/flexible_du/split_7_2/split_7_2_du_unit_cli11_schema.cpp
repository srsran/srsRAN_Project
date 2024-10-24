/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_7_2_du_unit_cli11_schema.h"
#include "apps/units/flexible_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "apps/units/flexible_du/o_du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_du/split_7_2/helpers/ru_ofh_config_cli11_schema.h"
#include "apps/units/flexible_du/support/cli11_cpu_affinities_parser_helper.h"
#include "split_7_2_du_unit_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

void srsran::configure_cli11_with_split_7_2_du_unit_config_schema(CLI::App& app, split_7_2_du_unit_config& parsed_cfg)
{
  configure_cli11_with_o_du_high_config_schema(app, parsed_cfg.odu_high_cfg);
  configure_cli11_with_du_low_config_schema(app, parsed_cfg.du_low_cfg);
  configure_cli11_with_ru_ofh_config_schema(app, parsed_cfg.ru_cfg);
}

void srsran::autoderive_split_7_2_du_parameters_after_parsing(CLI::App& app, split_7_2_du_unit_config& parsed_cfg)
{
  autoderive_o_du_high_parameters_after_parsing(app, parsed_cfg.odu_high_cfg);
  // Auto derive OFH parameters.
  autoderive_ru_ofh_parameters_after_parsing(app, parsed_cfg.ru_cfg);

  // Auto derive DU low parameters. Split 7.2 does not use blocking mode.
  const auto& cell = parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell;
  nr_band     band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_f_ref_arfcn);
  bool        is_blocking_mode_enabled = false;
  autoderive_du_low_parameters_after_parsing(app,
                                             parsed_cfg.du_low_cfg,
                                             band_helper::get_duplex_mode(band),
                                             is_blocking_mode_enabled,
                                             parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size());
}
