/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_config_validator.h"
#include "srsran/adt/interval.h"
#include "srsran/phy/upper/channel_processors/prach_detector_phy_validator.h"

using namespace srsran;

static bool validate_upper_phy_threads_appconfig(const du_low_unit_expert_threads_config& config,
                                                 unsigned                                 nof_hwacc_pdsch,
                                                 unsigned                                 nof_hwacc_pusch)
{
  static const interval<unsigned, true> max_concurrency_range(0, std::thread::hardware_concurrency());

  bool valid = true;

  if (!max_concurrency_range.contains(config.max_pucch_concurrency)) {
    fmt::print("Maximum PUCCH concurrency (i.e., {}) must be in range {}.\n",
               config.max_pucch_concurrency,
               max_concurrency_range);
    valid = false;
  }

  if (!max_concurrency_range.contains(config.max_pusch_and_srs_concurrency)) {
    fmt::print("Maximum joint PUSCH and SRS concurrency (i.e., {}) must be in range {}.\n",
               config.max_pusch_and_srs_concurrency,
               max_concurrency_range);
    valid = false;
  }

  if ((config.pdsch_processor_type != "auto") && (config.pdsch_processor_type != "flexible") &&
      (config.pdsch_processor_type != "generic")) {
    fmt::print("Invalid PDSCH processor type. Valid types are: auto, generic and flexible.\n");
    valid = false;
  }

  if (!max_concurrency_range.contains(config.max_pdsch_concurrency)) {
    fmt::print("Maximum PDSCH concurrency (i.e., {}) must be in range {}.\n",
               config.max_pdsch_concurrency,
               max_concurrency_range);
    valid = false;
  }

  return valid;
}

static bool validate_expert_execution_unit_config(const du_low_unit_config&        config,
                                                  const os_sched_affinity_bitmask& available_cpus)
{
  unsigned nof_hwacc_pdsch = 0;
  unsigned nof_hwacc_pusch = 0;
#ifdef DPDK_FOUND
  nof_hwacc_pdsch = config.hal_config->bbdev_hwacc->pdsch_enc->nof_hwacc;
  nof_hwacc_pusch = config.hal_config->bbdev_hwacc->pusch_dec->nof_hwacc;
#endif // DPDK_FOUND
  if (!validate_upper_phy_threads_appconfig(config.expert_execution_cfg.threads, nof_hwacc_pdsch, nof_hwacc_pusch)) {
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
  }
  return true;
}

static bool validate_phy_prach_configuration(span<const du_low_prach_validation_config>& prach_cells_config)
{
  for (const auto& prach_cell_cfg : prach_cells_config) {
    error_type<std::string> valid_prach = validate_prach_detector_phy(prach_cell_cfg.format,
                                                                      prach_cell_cfg.prach_scs,
                                                                      prach_cell_cfg.zero_correlation_zone,
                                                                      prach_cell_cfg.nof_prach_ports);
    if (!valid_prach.has_value()) {
      fmt::print("Invalid configuration:\n    {}", valid_prach.error());
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
                                    span<const du_low_prach_validation_config> prach_cells_config)
{
  // Configure more cells for expert execution than the number of cells is an error.
  if (config.expert_execution_cfg.cell_affinities.size() != prach_cells_config.size()) {
    fmt::print(
        "Using different number of cells for DU low expert execution '{}' than the number of defined cells '{}'\n",
        config.expert_execution_cfg.cell_affinities.size(),
        prach_cells_config.size());

    return false;
  }

  if (!validate_log_options(config.loggers, prach_cells_config.front())) {
    return false;
  }

  if (!validate_expert_phy_unit_config(config.expert_phy_cfg)) {
    return false;
  }

  if (!validate_phy_prach_configuration(prach_cells_config)) {
    return false;
  }

  return true;
}

bool srsran::validate_du_low_cpus(const du_low_unit_config& config, const os_sched_affinity_bitmask& available_cpus)
{
  return validate_expert_execution_unit_config(config, available_cpus);
}
