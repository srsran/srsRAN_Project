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

#pragma once

#include "apps/helpers/metrics/metrics_config.h"
#include "apps/services/worker_manager/os_sched_affinity_manager.h"
#include <string>

namespace srsran {

/// Expert SDR Radio Unit configuration.
struct ru_sdr_unit_expert_config {
  /// System time-based throttling. See \ref lower_phy_configuration::system_time_throttling for more information.
  float lphy_dl_throttling = 0.0F;
  /// \brief Selects the radio transmission mode.
  ///
  /// Selects the radio transmission mode between the available options:
  ///   - continuous: The radio keeps the transmitter chain active, even when there are no transmission requests.
  ///   - discontinuous: The transmitter stops when there is no data to transmit.
  ///   - same-port: like discontinuous mode, but using the same port to transmit and receive.
  ///
  /// \remark The discontinuous and same-port transmission modes may not be supported for some radio devices.
  std::string transmission_mode = "continuous";
  /// \brief Power ramping time of the transmit chain in microseconds.
  ///
  /// This parameter represents the duration, in microseconds, required for the transmit chain to reach its full power
  /// level.
  ///
  /// In discontinuous and same-port transmission modes, the transmitter is powered on ahead of the actual data
  /// transmission. By doing so, the data-carrying samples remain unaffected by any transient effects or fluctuations in
  /// the transmit chain during the power ramping time. The maximum supported power ramping time is equivalent to the
  /// duration of two NR slots.
  ///
  /// \note It is recommended to configure this parameter carefully, taking into account the characteristics of the
  /// transmit chain in order to achieve optimal performance.
  /// \note In same-port transmission mode, reception is interrupted on the TRX port as soon as the power ramping guard
  /// time starts.
  /// \note Powering up the transmitter ahead of time requires starting the transmission earlier, and reduces the time
  /// window for the radio to transmit the provided samples.
  float power_ramping_time_us = 0.0F;
};

/// Amplitude control application configuration.
struct amplitude_control_unit_config {
  /// Baseband gain back-off. This accounts for the signal PAPR and is applied regardless of clipping settings.
  float gain_backoff_dB = 12.0F;
  /// Power ceiling in dB, relative to the full scale amplitude of the radio.
  float power_ceiling_dBFS = -0.1F;
  /// Clipping of the baseband samples. If enabled, the samples that exceed the power ceiling are clipped.
  bool enable_clipping = false;
};

/// Configuration of logging functionalities.
struct ru_sdr_unit_logger_config {
  srslog::basic_levels radio_level = srslog::basic_levels::info;
  srslog::basic_levels phy_level   = srslog::basic_levels::warning;
};

/// CPU affinities configuration for the cell.
struct ru_sdr_unit_cpu_affinities_cell_config {
  /// Radio Unit workers CPU affinity mask.
  os_sched_affinity_config ru_cpu_cfg = {sched_affinity_mask_types::ru, {}, sched_affinity_mask_policy::mask};
  /// L1 downlink workers CPU affinity mask.
  os_sched_affinity_config l1_dl_cpu_cfg = {sched_affinity_mask_types::l1_dl, {}, sched_affinity_mask_policy::mask};
};

/// Lower physical layer thread profiles.
enum class lower_phy_thread_profile {
  /// Same task worker as the rest of the PHY (ZMQ only).
  blocking = 0,
  /// Single task worker for all the lower physical layer task executors.
  single,
  /// Two task workers - one for the downlink and one for the uplink.
  dual,
  /// Dedicated task workers for each of the subtasks (demodulation, reception and transmission).
  triple
};

/// Expert threads configuration of the SDR Radio Unit.
struct ru_sdr_unit_expert_threads_config {
  ru_sdr_unit_expert_threads_config()
  {
    unsigned nof_threads = cpu_architecture_info::get().get_host_nof_available_cpus();

    if (nof_threads < 4) {
      execution_profile = lower_phy_thread_profile::single;
    } else if (nof_threads < 8) {
      execution_profile = lower_phy_thread_profile::dual;
    } else {
      execution_profile = lower_phy_thread_profile::triple;
    }
  }

  /// Lower PHY thread configuration of the SDR Radio Unit.
  lower_phy_thread_profile execution_profile;
};

/// Expert configuration of the SDR Radio Unit.
struct ru_sdr_unit_expert_execution_config {
  /// Expert thread configuration of the SDR Radio Unit.
  ru_sdr_unit_expert_threads_config threads;
  /// \brief CPU affinities per cell of the SDR Radio Unit.
  ///
  /// \note Add one cell by default.
  std::vector<ru_sdr_unit_cpu_affinities_cell_config> cell_affinities = {{}};
};

/// Metrics configuration.
struct ru_sdr_unit_metrics_config {
  /// Metrics configuration.
  app_helpers::metrics_config metrics_cfg;
  /// Flag that control RU metrics.
  bool enable_ru_metrics = false;
};

/// SDR Radio Unit configuration.
struct ru_sdr_unit_config {
  /// Sampling frequency in MHz.
  double srate_MHz = 61.44;
  /// RF driver name.
  std::string device_driver = "uhd";
  /// RF driver arguments.
  std::string device_arguments = "";
  /// All transmit channel gain in decibels.
  double tx_gain_dB = 50.0;
  /// All receive channel gain in decibels.
  double rx_gain_dB = 60.0;
  /// Center frequency offset in hertz applied to all radio channels.
  double center_freq_offset_Hz = 0.0;
  /// Clock calibration in Parts Per Million (PPM). It is applied to the carrier frequency.
  double calibrate_clock_ppm = 0.0;
  /// LO Offset in MHz. It shifts the LO from the center frequency for moving the LO leakage out of the channel.
  double lo_offset_MHz = 0.0;
  /// \brief Rx to Tx radio time alignment calibration in samples.
  ///
  /// Compensates for the reception and transmission time misalignment inherent to the RF device. Setting this parameter
  /// overrides the default calibration, which is dependent on the selected RF device driver. Positive values reduce the
  /// RF transmission delay with respect to the RF reception. Since the UE receives the DL signal earlier, an incoming
  /// PRACH will also be detected earlier within the reception window. Negative values have the opposite effect, for
  /// example, a value of -1000 at a sample rate of 61.44 MHz increases the transmission delay and causes an incoming
  /// PRACH to be detected 16.3 us later within the reception window.
  std::optional<int> time_alignment_calibration;
  /// \brief Optional starting time.
  ///
  /// Starts radio operation at the given time. The use of this parameter is for starting radio operation of several
  /// instances at same time.
  std::optional<std::chrono::system_clock::time_point> start_time;
  /// Synchronization source.
  std::string synch_source = "default";
  /// Clock source.
  std::string clock_source = "default";
  /// Over-the wire format. Determines the format in which samples are transported from the radio to the host.
  std::string otw_format = "default";
  /// Loggers.
  ru_sdr_unit_logger_config loggers;
  /// Expert SDR Radio Unit settings.
  ru_sdr_unit_expert_config expert_cfg;
  /// Amplitude control configuration.
  amplitude_control_unit_config amplitude_cfg;
  /// Expert execution parameters for the SDR Radio Unit.
  ru_sdr_unit_expert_execution_config expert_execution_cfg;
  /// Metrics configuration.
  ru_sdr_unit_metrics_config metrics_cfg;
};

} // namespace srsran
