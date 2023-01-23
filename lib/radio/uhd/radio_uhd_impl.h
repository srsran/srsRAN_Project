/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_config_uhd_validator.h"
#include "radio_uhd_device.h"
#include "radio_uhd_device_type.h"
#include "radio_uhd_multi_usrp.h"
#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/radio/radio_management_plane.h"

namespace srsgnb {

/// Describes a radio session based on UHD that also implements the management and data plane functions.
class radio_session_uhd_impl : public radio_session,
                               private radio_management_plane,
                               private baseband_gateway,
                               private baseband_gateway_transmitter,
                               private baseband_gateway_receiver
{
private:
  /// Defines the start stream command delay in seconds.
  static constexpr double START_STREAM_DELAY_S = 0.1;
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
  /// Vector or transmit streams.
  static_vector<std::unique_ptr<radio_uhd_tx_stream>, RADIO_MAX_NOF_PORTS> tx_streams;
  /// Vector or receive streams.
  static_vector<std::unique_ptr<radio_uhd_rx_stream>, RADIO_MAX_NOF_PORTS> rx_streams;
  double                                                                   sampling_rate_hz;
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
  /// \param[out] is_locked Indicates if the sensor is locked by the time the method returns.
  /// \param[in] sensor_name Indicates the sensor name.
  /// \param[in] is_mboard Indicates if the sensor is from the motherboard or daughterboard.
  /// \param[in] timeout Indicates the amount of time to wait.
  /// \return True if no exception is caught. Otherwise false.
  bool wait_sensor_locked(bool& is_locked, const std::string& sensor_name, bool is_mboard, int timeout);

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
  /// \return True if no exception is caught. Otherwise false.
  bool start_rx_stream();

public:
  /// Constructs a radio session based on UHD.
  radio_session_uhd_impl(const radio_configuration::radio& radio_config,
                         task_executor&                    async_executor,
                         radio_notification_handler&       notifier_);

  /// \brief Indicates that the radio session was initialized succesfully.
  /// \return True if no exception is caught during initialization. Otherwise false.
  bool is_successful() const { return (state != states::UNINITIALIZED); }

  // See interface for documentation
  radio_management_plane& get_management_plane() override { return *this; }

  // See interface for documentation
  baseband_gateway& get_baseband_gateway() override { return *this; }

  // See interface for documentation
  void stop() override;

  // See interface for documentation
  baseband_gateway_transmitter& get_transmitter() override { return *this; }

  // See interface for documentation
  baseband_gateway_receiver& get_receiver() override { return *this; }

  // See interface for documentation
  void transmit(unsigned                                      stream_id,
                const baseband_gateway_transmitter::metadata& metadata,
                baseband_gateway_buffer&                      data) override;

  // See interface for documentation.
  baseband_gateway_receiver::metadata receive(baseband_gateway_buffer& data, unsigned stream_id) override;

  // See interface for documentation
  bool set_tx_gain(unsigned port_idx, double gain_dB) override { return set_tx_gain_unprotected(port_idx, gain_dB); }

  // See interface for documentation
  bool set_rx_gain(unsigned port_idx, double gain_dB) override { return set_rx_gain_unprotected(port_idx, gain_dB); }
};

class radio_factory_uhd_impl : public radio_factory
{
public:
  // See interface for documentation
  const radio_configuration::validator& get_configuration_validator() override { return config_validator; };

  // See interface for documentation
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_notification_handler&       notifier) override;

private:
  static radio_config_uhd_config_validator config_validator;
};

} // namespace srsgnb
