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

#include "ru_sdr_config_validator.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/phy_time_unit.h"

using namespace srsran;

/// Validates the given amplitude control application configuration. Returns true on success, otherwise false.
static bool validate_amplitude_control_unit_config(const amplitude_control_unit_config& config)
{
  bool valid = true;

  if (config.gain_backoff_dB < 0.0F) {
    fmt::print("Invalid gain backoff (i.e., {:.1f} dB). Must be positive or zero.\n", config.gain_backoff_dB);
    valid = false;
  }
  if (config.power_ceiling_dBFS > 0.0F) {
    fmt::print("Invalid power ceiling (i.e., {:.1f} dBFS). Must be negative or zero.\n", config.power_ceiling_dBFS);
    valid = false;
  }

  return valid;
}

/// Validates the given SDR Radio Unit application configuration. Returns true on success, otherwise false.
static bool validate_ru_sdr_appconfig(const ru_sdr_unit_config&                 config,
                                      span<const ru_sdr_cell_validation_config> cell_config)
{
  if (config.expert_execution_cfg.cell_affinities.size() != cell_config.size()) {
    fmt::println("Using different number of cells for SDR expert execution '{}' than the number of defined cells '{}'",
                 config.expert_execution_cfg.cell_affinities.size(),
                 cell_config.size());
    return false;
  }

  static constexpr phy_time_unit reference_time = phy_time_unit::from_units_of_kappa(16);

  if (!reference_time.is_sample_accurate(config.srate_MHz * 1e6)) {
    fmt::print("The sampling rate must be multiple of {:.2f} MHz.\n", 1e-6 / reference_time.to_seconds());
    return false;
  }

  const bool discontinuous_transmission = (config.expert_cfg.transmission_mode != "continuous");
  for (const auto& cell : cell_config) {
    if ((config.expert_cfg.transmission_mode == "same-port") && (cell.dplx_mode == duplex_mode::FDD)) {
      fmt::print("same-port transmission mode cannot be used with FDD cell configurations.\n");
      return false;
    }

    // Validates the sampling rate is compatible with the PRACH sequence.
    if (!cell.preamble_info.cp_length.is_sample_accurate(config.srate_MHz * 1e6)) {
      // List of common sampling rates for offering an alternative.
      static constexpr std::array<double, 10> sampling_rates = {
          7.68, 11.52, 15.36, 23.04, 30.76, 46.08, 61.44, 92.16, 122.88, 184.32};
      std::vector<double> valid_sampling_rates;
      for (double sampling_rate : sampling_rates) {
        // A valid alternative sampling rate must be larger than the cell bandwidth and be sample accurate with the
        // preamble cyclic prefix.
        if ((sampling_rate > bs_channel_bandwidth_to_MHz(cell.channel_bw_mhz)) &&
            (cell.preamble_info.cp_length.is_sample_accurate(sampling_rate * 1e6))) {
          valid_sampling_rates.push_back(sampling_rate);
        }
      }

      fmt::print("PRACH Format {} with subcarrier spacing of {} is not compatible with {:.2f}MHz sampling rate. "
                 "Valid sampling rates are {:,} MHz.\n",
                 to_string(cell.prach_format),
                 to_string(cell.common_scs),
                 config.srate_MHz,
                 span<const double>(valid_sampling_rates));
      return false;
    }

    static constexpr float subframe_duration_us = 1e3;
    float slot_duration_us = subframe_duration_us / static_cast<float>(get_nof_slots_per_subframe(cell.common_scs));
    if (discontinuous_transmission && (config.expert_cfg.power_ramping_time_us > 2 * slot_duration_us)) {
      fmt::print("Power ramping time, i.e., {:.1f} us, cannot exceed the duration of two NR slots, i.e., {:.1f} us.\n",
                 config.expert_cfg.power_ramping_time_us,
                 2 * slot_duration_us);
      return false;
    }
  }

  static constexpr interval<float> lphy_dl_throttling_range(0.0F, 1.0F);
  if (!lphy_dl_throttling_range.contains(config.expert_cfg.lphy_dl_throttling)) {
    fmt::print("Low PHY throttling (i.e., {}) must be in range {}.\n",
               config.expert_cfg.lphy_dl_throttling,
               lphy_dl_throttling_range);
    return false;
  }

  if (discontinuous_transmission && (config.device_driver == "zmq")) {
    fmt::print("Discontinuous transmission modes cannot be used with ZMQ.\n");

    return false;
  }

  if (config.expert_cfg.power_ramping_time_us < 0) {
    fmt::print("Power ramping time, i.e., {:.1f} us, must be positive or zero.\n",
               config.expert_cfg.power_ramping_time_us);
    return false;
  }

  if (!validate_amplitude_control_unit_config(config.amplitude_cfg)) {
    return false;
  }

  return true;
}

bool srsran::validate_ru_sdr_config(const ru_sdr_unit_config&                 config,
                                    span<const ru_sdr_cell_validation_config> cell_config)
{
  if (!validate_ru_sdr_appconfig(config, cell_config)) {
    return false;
  }
  if (config.srate_MHz < bs_channel_bandwidth_to_MHz(cell_config.front().channel_bw_mhz)) {
    fmt::print("Sampling rate (i.e. {} MHz) is too low for the requested channel bandwidth (i.e. {} MHz).\n",
               config.srate_MHz,
               bs_channel_bandwidth_to_MHz(cell_config.front().channel_bw_mhz));
    return false;
  }

  return true;
}
