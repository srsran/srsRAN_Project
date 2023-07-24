/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "radio_config_uhd_validator.h"
#include "radio_uhd_baseband_gateway.h"
#include "radio_uhd_device.h"
#include "radio_uhd_device_type.h"
#include "radio_uhd_multi_usrp.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/radio/radio_management_plane.h"

namespace srsran {

/// Describes a radio session based on UHD that also implements the management and data plane functions.
class radio_session_uhd_impl : public radio_session, private radio_management_plane
{
private:
  /// Wait at most 1s for external clock locking.
  static constexpr int CLOCK_TIMEOUT_MS = 1000;
  /// Enumerates possible UHD session states.
  enum class states { UNINITIALIZED, SUCCESSFUL_INIT, STOP };
  /// Maps ports to stream and channel indexes.
  using port_to_stream_channel = std::pair<unsigned, unsigned>;
  /// Indicates the current state.
  std::atomic<states> state;
  /// Wraps the UHD device functions.
  radio_uhd_device device;
  /// Indexes the transmitter port indexes into stream and channel index as first and second.
  static_vector<port_to_stream_channel, RADIO_MAX_NOF_PORTS> tx_port_map;
  /// Indexes the receiver port indexes into stream and channel index as first and second.
  static_vector<port_to_stream_channel, RADIO_MAX_NOF_PORTS> rx_port_map;
  /// Baseband gateways.
  std::vector<std::unique_ptr<radio_uhd_baseband_gateway>> bb_gateways;
  double                                                   actual_sampling_rate_Hz;
  /// Protects the stream start.
  std::mutex stream_start_mutex;
  /// Indicates if the reception streams require start.
  bool stream_start_required = true;
  /// Asynchronous executor.
  task_executor& async_executor;
  /// Event notifier.
  radio_notification_handler& notifier;

  /// \brief Set the synchronization time to GPS mode.
  /// \return True if no exception is caught. Otherwise false.
  bool set_time_to_gps_time();

  /// \brief Waits for a sensor to be locked.
  /// \param[in] sensor_name Indicates the sensor name.
  /// \param[in] is_mboard Indicates if the sensor is from the motherboard or daughterboard.
  /// \param[in] timeout Indicates the amount of time to wait.
  /// \return True if sensor is found and locked. Otherwise false.
  bool wait_sensor_locked(const std::string& sensor_name, bool is_mboard, int timeout);

  /// \brief Set transmission gain from the class itself.
  /// \param[in] port_idx Indicates the port index.
  /// \param[in] gain_dB Indicates the gain value.
  /// \return True if the port index and gain value are valid, and no exception is caught. Otherwise false.
  bool set_tx_gain_unprotected(unsigned port_idx, double gain_dB);

  /// \brief Set reception gain from the class itself.
  /// \param[in] port_idx Indicates the port index.
  /// \param[in] gain_dB Indicates the gain value.
  /// \return True if the port index and gain value are valid, and no exception is caught. Otherwise false.
  bool set_rx_gain_unprotected(unsigned port_idx, double gain_dB);

  /// \brief Set transmission frequency.
  /// \param[in] port_idx Indicates the port index.
  /// \param[in] frequency Provides the frequency tuning parameters
  /// \return True if the port index and frequency value are valid, and no exception is caught. Otherwise false.
  bool set_tx_freq(unsigned port_idx, radio_configuration::lo_frequency frequency);

  /// \brief Set reception frequency.
  /// \param[in] port_idx Indicates the port index.
  /// \param[in] frequency Provides the frequency tuning parameters
  /// \return True if the port index and frequency value are valid, and no exception is caught. Otherwise false.
  bool set_rx_freq(unsigned port_idx, radio_configuration::lo_frequency frequency);

  /// \brief Start receive streams.
  /// \param[in] init_time Time in which the stream shall start.
  /// \return True if no exception is caught. Otherwise false.
  bool start_rx_stream(baseband_gateway_timestamp init_time);

public:
  baseband_gateway_timestamp read_current_time() override;

public:
  /// Constructs a radio session based on UHD.
  radio_session_uhd_impl(const radio_configuration::radio& radio_config,
                         task_executor&                    async_executor,
                         radio_notification_handler&       notifier_);

  /// \brief Indicates that the radio session was initialized succesfully.
  /// \return True if no exception is caught during initialization. Otherwise false.
  bool is_successful() const { return (state != states::UNINITIALIZED); }

  // See interface for documentation.
  radio_management_plane& get_management_plane() override { return *this; }

  // See interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    srsran_assert(stream_id < bb_gateways.size(),
                  "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {})",
                  stream_id,
                  bb_gateways.size());
    return *bb_gateways[stream_id];
  }

  // See interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  bool set_tx_gain(unsigned port_idx, double gain_dB) override { return set_tx_gain_unprotected(port_idx, gain_dB); }

  // See interface for documentation.
  bool set_rx_gain(unsigned port_idx, double gain_dB) override { return set_rx_gain_unprotected(port_idx, gain_dB); }
};

class radio_factory_uhd_impl : public radio_factory
{
public:
  // See interface for documentation.
  const radio_configuration::validator& get_configuration_validator() override { return config_validator; };

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_notification_handler&       notifier) override;

private:
  static radio_config_uhd_config_validator config_validator;
};

} // namespace srsran
