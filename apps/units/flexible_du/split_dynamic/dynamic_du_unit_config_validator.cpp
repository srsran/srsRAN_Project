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

#include "dynamic_du_unit_config_validator.h"
#include "apps/units/flexible_du/du_high/du_high_config_validator.h"
#include "apps/units/flexible_du/du_low/du_low_config_validator.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_config_validator.h"
#include "apps/units/flexible_du/split_8/ru_sdr_config_validator.h"
#include "srsran/ran/prach/prach_configuration.h"

using namespace srsran;

static std::vector<du_low_prach_validation_config> get_du_low_validation_dependencies(const du_high_unit_config& config)
{
  std::vector<du_low_prach_validation_config> out_cfg(config.cells_cfg.size());

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    du_low_prach_validation_config&      out_cell = out_cfg[i];
    const du_high_unit_base_cell_config& in_cell  = config.cells_cfg[i].cell;

    // Get PRACH info.
    subcarrier_spacing  common_scs = in_cell.common_scs;
    prach_configuration prach_info = prach_configuration_get(frequency_range::FR1,
                                                             band_helper::get_duplex_mode(in_cell.band.value()),
                                                             in_cell.prach_cfg.prach_config_index.value());

    // PRACH format type.
    out_cell.format = prach_info.format;

    // Get preamble info.
    prach_preamble_information preamble_info =
        is_long_preamble(prach_info.format)
            ? get_prach_preamble_long_info(prach_info.format)
            : get_prach_preamble_short_info(prach_info.format, to_ra_subcarrier_spacing(common_scs), false);

    out_cell.prach_scs             = preamble_info.scs;
    out_cell.zero_correlation_zone = in_cell.prach_cfg.zero_correlation_zone;
    out_cell.nof_prach_ports       = in_cell.prach_cfg.ports.size();
    out_cell.nof_antennas_ul       = in_cell.nof_antennas_ul;
  }

  return out_cfg;
}

static std::vector<ru_ofh_cell_validation_config> get_ru_ofh_validation_dependencies(const du_high_unit_config& config)
{
  std::vector<ru_ofh_cell_validation_config> out_cfg(config.cells_cfg.size());

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    ru_ofh_cell_validation_config&       out_cell = out_cfg[i];
    const du_high_unit_base_cell_config& in_cell  = config.cells_cfg[i].cell;

    // Validates the sampling rate is compatible with the PRACH sequence.
    out_cell.scs             = in_cell.common_scs;
    out_cell.nof_prach_ports = in_cell.prach_cfg.ports.size();
    out_cell.nof_antennas_dl = in_cell.nof_antennas_dl;
    out_cell.nof_antennas_ul = in_cell.nof_antennas_ul;
  }

  return out_cfg;
}

static std::vector<ru_sdr_cell_validation_config> get_ru_sdr_validation_dependencies(const du_high_unit_config& config)
{
  std::vector<ru_sdr_cell_validation_config> out_cfg(config.cells_cfg.size());

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    ru_sdr_cell_validation_config&       out_cell = out_cfg[i];
    const du_high_unit_base_cell_config& in_cell  = config.cells_cfg[i].cell;

    // Validates the sampling rate is compatible with the PRACH sequence.
    out_cell.common_scs            = in_cell.common_scs;
    prach_configuration prach_info = prach_configuration_get(frequency_range::FR1,
                                                             band_helper::get_duplex_mode(in_cell.band.value()),
                                                             in_cell.prach_cfg.prach_config_index.value());
    out_cell.prach_format          = prach_info.format;
    out_cell.channel_bw_mhz        = in_cell.channel_bw_mhz;
    out_cell.dplx_mode             = band_helper::get_duplex_mode(in_cell.band.value());
    out_cell.preamble_info =
        is_long_preamble(prach_info.format)
            ? get_prach_preamble_long_info(prach_info.format)
            : get_prach_preamble_short_info(prach_info.format, to_ra_subcarrier_spacing(in_cell.common_scs), false);
  }

  return out_cfg;
}

static bool validate_expert_execution_unit_config(const ru_dummy_unit_config&      config,
                                                  const os_sched_affinity_bitmask& available_cpus)
{
  auto validate_cpu_range = [](const os_sched_affinity_bitmask& allowed_cpus_mask,
                               const os_sched_affinity_bitmask& mask,
                               const std::string&               name) {
    auto invalid_cpu_ids = mask.subtract(allowed_cpus_mask);
    if (not invalid_cpu_ids.empty()) {
      fmt::print("CPU cores {} selected in '{}' option doesn't belong to available cpuset.\n", invalid_cpu_ids, name);
      return false;
    }

    return true;
  };

  for (const auto& cell : config.cell_affinities) {
    if (!validate_cpu_range(available_cpus, cell.ru_cpu_cfg.mask, "ru_cpus")) {
      return false;
    }
  }
  return true;
}

bool srsran::validate_dynamic_du_unit_config(const dynamic_du_unit_config&    config,
                                             const os_sched_affinity_bitmask& available_cpus)
{
  if (!validate_du_high_config(config.du_high_cfg.config, available_cpus)) {
    return false;
  }

  auto du_low_dependencies = get_du_low_validation_dependencies(config.du_high_cfg.config);
  if (!validate_du_low_config(config.du_low_cfg, du_low_dependencies, available_cpus)) {
    return false;
  }

  if (std::holds_alternative<ru_ofh_unit_parsed_config>(config.ru_cfg)) {
    auto ru_ofh_dependencies = get_ru_ofh_validation_dependencies(config.du_high_cfg.config);
    return validate_ru_ofh_config(
        std::get<ru_ofh_unit_parsed_config>(config.ru_cfg).config, ru_ofh_dependencies, available_cpus);
  }

  if (std::holds_alternative<ru_sdr_unit_config>(config.ru_cfg)) {
    auto ru_sdr_dependencies = get_ru_sdr_validation_dependencies(config.du_high_cfg.config);
    return validate_ru_sdr_config(std::get<ru_sdr_unit_config>(config.ru_cfg), ru_sdr_dependencies, available_cpus);
  }

  if (!validate_expert_execution_unit_config(std::get<ru_dummy_unit_config>(config.ru_cfg), available_cpus)) {
    return false;
  }

  return true;
}
