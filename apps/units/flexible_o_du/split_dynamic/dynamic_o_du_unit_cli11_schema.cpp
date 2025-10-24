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

#include "dynamic_o_du_unit_cli11_schema.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/services/worker_manager/cli11_cpu_affinities_parser_helper.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_cli11_schema.h"
#include "dynamic_o_du_unit_config.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static ru_ofh_unit_parsed_config ofh_cfg;
static ru_sdr_unit_config        sdr_cfg;
static ru_dummy_unit_config      dummy_cfg;

static void configure_cli11_ru_dummy_args(CLI::App& app, ru_dummy_unit_config& config)
{
  add_option(app, "--dl_processing_delay", config.dl_processing_delay, "DL processing processing delay in slots")
      ->capture_default_str();
  add_option(app,
             "--time_scaling",
             config.time_scaling,
             "Time scaling factor applied to the slot duration. Must be greater than zero. "
             "A value greater than one slows down the RU, while a value between zero and one speeds it up.")
      ->capture_default_str()
      ->check(CLI::NonNegativeNumber);
}

static void configure_cli11_cell_affinity_args(CLI::App& app, ru_dummy_cpu_affinities_cell_unit_config& config)
{
  add_option_function<std::string>(
      app,
      "--ru_cpus",
      [&config](const std::string& value) { parse_affinity_mask(config.ru_cpu_cfg.mask, value, "ru_cpus"); },
      "Number of CPUs used for the Radio Unit tasks");

  app.add_option_function<std::string>(
      "--ru_pinning",
      [&config](const std::string& value) {
        config.ru_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.ru_cpu_cfg.pinning_policy == sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "ru_pinning");
        }
      },
      "Policy used for assigning CPU cores to the Radio Unit tasks");
}

static void configure_cli11_expert_execution_args(CLI::App& app, ru_dummy_unit_config& config)
{
  // Cell affinity section.
  add_option_cell(
      app,
      "--cell_affinities",
      [&config](const std::vector<std::string>& values) {
        config.cell_affinities.resize(values.size());

        for (unsigned i = 0, e = values.size(); i != e; ++i) {
          CLI::App subapp("Dummy RU expert execution cell CPU affinities",
                          "Dummy RUexpert execution cell CPU affinities config, item #" + std::to_string(i));
          subapp.config_formatter(create_yaml_config_parser());
          subapp.allow_config_extras();
          configure_cli11_cell_affinity_args(subapp, config.cell_affinities[i]);
          std::istringstream ss(values[i]);
          subapp.parse_from_stream(ss);
        }
      },
      "Sets the cell CPU affinities configuration on a per cell basis");
}

static void configure_cli11_metrics_args(CLI::App& app, ru_dummy_unit_metrics_config& config)
{
  CLI::App* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd, "--enable_ru", config.enable_ru_metrics, "Enable Radio Unit metrics");
}

void srsran::configure_cli11_with_dynamic_o_du_unit_config_schema(CLI::App& app, dynamic_o_du_unit_config& parsed_cfg)
{
  configure_cli11_with_o_du_high_config_schema(app, parsed_cfg.odu_high_cfg);
  configure_cli11_with_du_low_config_schema(app, parsed_cfg.du_low_cfg);
  configure_cli11_with_ru_ofh_config_schema(app, ofh_cfg);
  configure_cli11_with_ru_sdr_config_schema(app, sdr_cfg);

  CLI::App* ru_dummy_subcmd = add_subcommand(app, "ru_dummy", "Dummy Radio Unit configuration")->configurable();
  configure_cli11_ru_dummy_args(*ru_dummy_subcmd, dummy_cfg);

  // Metrics section.
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, dummy_cfg.metrics_cfg.metrics_cfg);
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, dummy_cfg.metrics_cfg);

  // Expert execution section.
  CLI::App* expert_subcmd = add_subcommand(app, "expert_execution", "Expert execution configuration")->configurable();
  configure_cli11_expert_execution_args(*expert_subcmd, dummy_cfg);
}

static void manage_ru(CLI::App& app, dynamic_o_du_unit_config& parsed_cfg)
{
  // Manage the RU optionals
  auto     ofh_subcmd        = app.get_subcommand("ru_ofh");
  auto     sdr_subcmd        = app.get_subcommand("ru_sdr");
  auto     dummy_subcmd      = app.get_subcommand("ru_dummy");
  unsigned nof_ofh_entries   = ofh_subcmd->count_all();
  unsigned nof_sdr_entries   = sdr_subcmd->count_all();
  unsigned nof_dummy_entries = dummy_subcmd->count_all();

  // Count the number of RU types.
  unsigned nof_ru_types = (nof_ofh_entries != 0) ? 1 : 0;
  nof_ru_types += (nof_sdr_entries != 0) ? 1 : 0;
  nof_ru_types += (nof_dummy_entries != 0) ? 1 : 0;

  if (nof_ru_types > 1) {
    srsran_terminate("Radio Unit configuration allows either a SDR, Open Fronthaul, or Dummy configuration, but not "
                     "different types of them at the same time");
  }

  if (nof_ofh_entries != 0) {
    parsed_cfg.ru_cfg = ofh_cfg;
    sdr_subcmd->disabled();
    dummy_subcmd->disabled();

    return;
  }

  if (nof_sdr_entries != 0) {
    parsed_cfg.ru_cfg = sdr_cfg;
    ofh_subcmd->disabled();
    dummy_subcmd->disabled();

    return;
  }

  parsed_cfg.ru_cfg = dummy_cfg;
  sdr_subcmd->disabled();
  ofh_subcmd->disabled();
}

void srsran::autoderive_dynamic_o_du_parameters_after_parsing(CLI::App& app, dynamic_o_du_unit_config& parsed_cfg)
{
  const unsigned nof_cells = parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  autoderive_o_du_high_parameters_after_parsing(app, parsed_cfg.odu_high_cfg);
  // Auto derive SDR parameters.
  autoderive_ru_sdr_parameters_after_parsing(app, sdr_cfg, nof_cells);
  // Auto derive OFH parameters.
  autoderive_ru_ofh_parameters_after_parsing(app, ofh_cfg);

  // Set the parsed RU.
  manage_ru(app, parsed_cfg);

  if (std::holds_alternative<ru_dummy_unit_config>(parsed_cfg.ru_cfg)) {
    auto& dummy = std::get<ru_dummy_unit_config>(parsed_cfg.ru_cfg);
    if (dummy.cell_affinities.size() < nof_cells) {
      dummy.cell_affinities.resize(nof_cells);
    }
  }

  // Auto derive DU low parameters.
  const auto&   cell = parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell;
  const nr_band band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_f_ref_arfcn);
  autoderive_du_low_parameters_after_parsing(app, parsed_cfg.du_low_cfg, band_helper::get_duplex_mode(band));
}
