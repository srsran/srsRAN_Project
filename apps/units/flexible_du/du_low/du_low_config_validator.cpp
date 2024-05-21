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

#include "du_low_config_validator.h"
#include "srsran/adt/interval.h"
#include "srsran/phy/upper/channel_processors/prach_detector_phy_validator.h"

using namespace srsran;

static bool validate_upper_phy_threads_appconfig(const du_low_unit_expert_threads_config& config,
                                                 unsigned                                 max_processing_delay_slots)
{
  static const interval<unsigned, true> nof_ul_dl_threads_range(1, std::thread::hardware_concurrency());
  static const interval<unsigned, true> nof_pdsch_threads_range(2, std::thread::hardware_concurrency());

  bool valid = true;

  if (!nof_ul_dl_threads_range.contains(config.nof_ul_threads)) {
    fmt::print(
        "Number of PHY UL threads (i.e., {}) must be in range {}.\n", config.nof_ul_threads, nof_ul_dl_threads_range);
    valid = false;
  }

  if ((config.pdsch_processor_type != "auto") && (config.pdsch_processor_type != "concurrent") &&
      config.pdsch_processor_type != "generic" && (config.pdsch_processor_type != "lite")) {
    fmt::print("Invalid PDSCH processor type. Valid types are: auto, generic, concurrent and lite.\n");
    valid = false;
  }

  if ((config.pdsch_processor_type == "concurrent") && !nof_pdsch_threads_range.contains(config.nof_dl_threads)) {
    fmt::print("For concurrent PDSCH processor. Number of PHY DL threads (i.e., {}) must be in range {}.\n",
               config.nof_dl_threads,
               nof_pdsch_threads_range);
    valid = false;
  }

  if (!nof_ul_dl_threads_range.contains(config.nof_dl_threads)) {
    fmt::print(
        "Number of PHY DL threads (i.e., {}) must be in range {}.\n", config.nof_dl_threads, nof_ul_dl_threads_range);
    valid = false;
  }

  return valid;
}

static bool validate_expert_execution_unit_config(const du_low_unit_config&        config,
                                                  unsigned                         nof_cells,
                                                  const os_sched_affinity_bitmask& available_cpus)
{
  if (!validate_upper_phy_threads_appconfig(config.expert_execution_cfg.threads,
                                            config.expert_phy_cfg.max_processing_delay_slots)) {
    return false;
  }

  // Configure more cells for expert execution than the number of cells is an error.
  if (config.expert_execution_cfg.cell_affinities.size() != nof_cells) {
    fmt::print(
        "Using different number of cells for DU low expert execution '{}' than the number of defined cells '{}'\n",
        config.expert_execution_cfg.cell_affinities.size(),
        nof_cells);

    return false;
  }

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

  for (const auto& cell : config.expert_execution_cfg.cell_affinities) {
    if (!validate_cpu_range(available_cpus, cell.l1_dl_cpu_cfg.mask, "l1_dl_cpus")) {
      return false;
    }
    if (!validate_cpu_range(available_cpus, cell.l1_ul_cpu_cfg.mask, "l1_ul_cpus")) {
      return false;
    }
  }
  return true;
}

static bool validate_phy_prach_configuration(span<const du_low_prach_validation_config>& prach_cells_config)
{
  for (const auto& prach_cell_cfg : prach_cells_config) {
    if (!validate_prach_detector_phy(prach_cell_cfg.format,
                                     prach_cell_cfg.prach_scs,
                                     prach_cell_cfg.zero_correlation_zone,
                                     prach_cell_cfg.nof_prach_ports)) {
      return false;
    }
  }

  return true;
}

/// Validates expert physical layer configuration parameters.
static bool validate_expert_phy_unit_config(const du_low_unit_expert_upper_phy_config& config)
{
  bool valid = true;

  if ((config.pusch_sinr_calc_method != "channel_estimator") &&
      (config.pusch_sinr_calc_method != "post_equalization") && (config.pusch_sinr_calc_method != "evm")) {
    fmt::print(
        "Invalid PUSCH SINR calculation method. Valid types are: channel_estimator, post_equalization and evm.\n");
    valid = false;
  }

  if (config.pusch_decoder_max_iterations == 0) {
    fmt::print("Maximum PUSCH LDPC decoder iterations cannot be zero.\n");
    valid = false;
  }

  return valid;
}

static bool validate_log_options(const du_low_unit_logger_config& config, const du_low_prach_validation_config& cell)
{
  if (!config.phy_rx_symbols_filename.empty() && config.phy_rx_symbols_port.has_value() &&
      (config.phy_rx_symbols_port.value() >= cell.nof_antennas_ul)) {
    fmt::print("Requested IQ dump from Rx port {}, valid Rx ports are 0-{}.\n",
               config.phy_rx_symbols_port.value(),
               cell.nof_antennas_ul - 1);
    return false;
  }

  return true;
}

bool srsran::validate_du_low_config(const du_low_unit_config&                  config,
                                    span<const du_low_prach_validation_config> prach_cells_config,
                                    const os_sched_affinity_bitmask&           available_cpus)
{
  if (!validate_log_options(config.loggers, prach_cells_config.front())) {
    return false;
  }

  if (!validate_expert_phy_unit_config(config.expert_phy_cfg)) {
    return false;
  }

  if (!validate_phy_prach_configuration(prach_cells_config)) {
    return false;
  }

  if (!validate_expert_execution_unit_config(config, prach_cells_config.size(), available_cpus)) {
    return false;
  }

  return true;
}
