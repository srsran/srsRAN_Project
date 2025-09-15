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
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_cli11_schema.h"
#include "split6_constants.h"
#include "split6_o_du_low_unit_config.h"
#include "srsran/ran/band_helper.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static ru_ofh_unit_parsed_config ofh_cfg;
static ru_sdr_unit_config        sdr_cfg;

void srsran::configure_cli11_with_split6_o_du_low_unit_config_schema(CLI::App& app, split6_o_du_low_unit_config& config)
{
  configure_cli11_with_du_low_config_schema(app, config.du_low_cfg);
  configure_cli11_with_ru_ofh_config_schema(app, ofh_cfg);
  configure_cli11_with_ru_sdr_config_schema(app, sdr_cfg);

  CLI::App* logger_subcmd = add_subcommand(app, "log", "Logger configuration")->configurable();
  app_helpers::add_log_option(*logger_subcmd, config.fapi_level, "--fapi_level", "FAPI log level");

  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  auto*     periodicity_subcmd =
      add_subcommand(*metrics_subcmd, "periodicity", "Metrics periodicity configuration")->configurable();
  add_option(
      *periodicity_subcmd, "--du_report_period", config.du_report_period, "DU statistics report period in milliseconds")
      ->capture_default_str()
      ->check(CLI::Range(0, 10240));
}

static void manage_ru(const CLI::App& app, split6_o_du_low_unit_config& config)
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
  if (CLI::App* expert_cmd = app.get_subcommand("expert_phy");
      expert_cmd->count_all() == 0 || expert_cmd->count("--max_proc_delay") == 0) {
    report_error("'max_proc_delay' property is mandatory in this configuration");
  }

  // Auto derive SDR parameters.
  autoderive_ru_sdr_parameters_after_parsing(app, sdr_cfg, split6_du_low::NOF_CELLS_SUPPORTED);
  // Auto derive OFH parameters.
  autoderive_ru_ofh_parameters_after_parsing(app, ofh_cfg);

  // Set the parsed RU.
  manage_ru(app, config);

  // NOTE: TDD is hardcoded because it does not matter as max proc delay parameter is mandatory in this application
  // unit.
  autoderive_du_low_parameters_after_parsing(
      app, config.du_low_cfg, duplex_mode::TDD, split6_du_low::NOF_CELLS_SUPPORTED);
}
