/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_8_du_unit_cli11_schema.h"
#include "apps/units/flexible_du/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_du/du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_du/fapi/fapi_config_cli11_schema.h"
#include "apps/units/flexible_du/split_8/helpers/ru_sdr_config_cli11_schema.h"
#include "split_8_du_unit_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

void srsran::configure_cli11_with_split_8_du_unit_config_schema(CLI::App& app, split_8_du_unit_config& parsed_cfg)
{
  configure_cli11_with_du_high_config_schema(app, parsed_cfg.du_high_cfg);
  configure_cli11_with_du_low_config_schema(app, parsed_cfg.du_low_cfg);
  configure_cli11_with_fapi_config_schema(app, parsed_cfg.fapi_cfg);
  configure_cli11_with_ru_sdr_config_schema(app, parsed_cfg.ru_cfg);
}

void srsran::autoderive_split_8_du_parameters_after_parsing(CLI::App& app, split_8_du_unit_config& parsed_cfg)
{
  autoderive_du_high_parameters_after_parsing(app, parsed_cfg.du_high_cfg.config);
  // Auto derive SDR parameters.
  autoderive_ru_sdr_parameters_after_parsing(app, parsed_cfg.ru_cfg, parsed_cfg.du_high_cfg.config.cells_cfg.size());

  // Auto derive DU low parameters.
  const auto& cell = parsed_cfg.du_high_cfg.config.cells_cfg.front().cell;
  nr_band     band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_f_ref_arfcn);
  bool        is_blocking_mode_enabled = parsed_cfg.ru_cfg.device_driver == "zmq";

  autoderive_du_low_parameters_after_parsing(app,
                                             parsed_cfg.du_low_cfg,
                                             band_helper::get_duplex_mode(band),
                                             is_blocking_mode_enabled,
                                             parsed_cfg.du_high_cfg.config.cells_cfg.size());
}
