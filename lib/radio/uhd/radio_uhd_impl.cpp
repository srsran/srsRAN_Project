/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "radio_uhd_impl.h"
#include "radio_uhd_device.h"

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__
#include <uhd/utils/thread_priority.h>
#pragma GCC diagnostic pop

using namespace srsgnb;

bool radio_session_uhd_impl::set_time_to_gps_time()
{
  const std::string sensor_name = "gps_time";

  std::vector<std::string> sensors;
  if (device.get_mboard_sensor_names(sensors) != UHD_ERROR_NONE) {
    printf("Error: not possible to read sensors. %s.\n", device.get_error_message().c_str());
    return false;
  }

  // Find sensor name. Error if it is not available.
  if (std::find(sensors.begin(), sensors.end(), sensor_name) == sensors.end()) {
    printf("Error: sensor %s not found\n", sensor_name.c_str());
    return false;
  }

  // Get actual sensor value
  double frac_secs = 0.0;
  if (!device.get_sensor(sensor_name, frac_secs)) {
    printf("Error: not possible to read sensor %s. %s.\n", sensor_name.c_str(), device.get_error_message().c_str());
    return false;
  }

  // Get time and set
  printf("Setting USRP time to %fs\n", frac_secs);
  if (device.set_time_unknown_pps(uhd::time_spec_t(frac_secs)) != UHD_ERROR_NONE) {
    printf("Error: failed to set time. %s.\n", device.get_error_message().c_str());
    return false;
  }

  return true;
}

bool radio_session_uhd_impl::wait_sensor_locked(bool&              is_locked,
                                                const std::string& sensor_name,
                                                bool               is_mboard,
                                                int                timeout)
{
  is_locked = false;

  // Get sensor list
  std::vector<std::string> sensors;
  if (is_mboard) {
    // motherboard sensor
    if (!device.get_mboard_sensor_names(sensors)) {
      printf("Error: getting mboard sensor names. %s.", device.get_error_message().c_str());
      return false;
    }
  } else {
    // daughterboard sensor
    if (!device.get_rx_sensor_names(sensors)) {
      printf("Error: getting Rx sensor names. %s.", device.get_error_message().c_str());
      return false;
    }
  }

  // Find sensor name. Error if it is not available.
  if (std::find(sensors.begin(), sensors.end(), sensor_name) == sensors.end()) {
    printf("Error: sensor %s not found\n", sensor_name.c_str());
    return false;
  }

  do {
    // Get actual sensor value
    if (is_mboard) {
      if (!device.get_sensor(sensor_name, is_locked)) {
        printf("Error: reading mboard sensor %s. %s.\n", sensor_name.c_str(), device.get_error_message().c_str());
        return false;
      }
    } else {
      if (!device.get_rx_sensor(sensor_name, is_locked)) {
        printf("Error: reading rx sensor %s. %s.\n", sensor_name.c_str(), device.get_error_message().c_str());
        return false;
      }
    }

    // Read value and wait
    usleep(1000); // 1ms
    timeout -= 1; // 1ms
  } while (not is_locked and timeout > 0);

  return true;
}

bool radio_session_uhd_impl::set_tx_gain_unprotected(unsigned int port_idx, double gain_dB)
{
  if (port_idx >= tx_port_map.size()) {
    printf("Error: transmit port index (%d) exceeds the number of ports (%d).\n", port_idx, (int)tx_port_map.size());
    return false;
  }

  uhd::gain_range_t range;
  if (!device.get_tx_gain_range(range, port_idx)) {
    printf("Error: getting gain range for transmitter ch=%d. %s.\n", port_idx, device.get_error_message().c_str());
    return false;
  }

  // Check gain range.
  if (gain_dB > range.stop() || gain_dB < range.start()) {
    printf("Error: transmitter %d gain %.2f is out-of-range (%.2f to %.2f)\n",
           port_idx,
           gain_dB,
           range.start(),
           range.stop());
    return false;
  }

  // Setup gain.
  if (!device.set_tx_gain(port_idx, gain_dB)) {
    printf("Error: setting gain for transmitter %d. %s.\n", port_idx, device.get_error_message().c_str());
  }

  return true;
}

bool radio_session_uhd_impl::set_rx_gain_unprotected(unsigned int port_idx, double gain_dB)
{
  if (port_idx >= rx_port_map.size()) {
    printf("Error: receive port index (%d) exceeds the number of ports (%d).\n", port_idx, (int)rx_port_map.size());
    return false;
  }

  uhd::gain_range_t range;
  if (!device.get_rx_gain_range(range, port_idx)) {
    printf("Error: getting gain range for receiver ch=%d. %s.\n", port_idx, device.get_error_message().c_str());
    return false;
  }

  // Check gain range.
  if (gain_dB > range.stop() || gain_dB < range.start()) {
    printf("Error: receiver %d gain %.2f is out-of-range (%.2f to %.2f)\n",
           port_idx,
           gain_dB,
           range.start(),
           range.stop());
    return false;
  }

  // Setup gain.
  if (!device.set_rx_gain(port_idx, gain_dB)) {
    printf("Error: setting gain for receiver %d. %s.\n", port_idx, device.get_error_message().c_str());
    return false;
  }

  return true;
}

bool radio_session_uhd_impl::set_tx_freq(unsigned int port_idx, radio_configuration::lo_frequency frequency)
{
  if (port_idx >= tx_port_map.size()) {
    printf("Error: transmit port index (%d) exceeds the number of ports (%d).\n", port_idx, (int)tx_port_map.size());
    return false;
  }

  uhd::freq_range_t range;
  if (!device.get_tx_freq_range(range, port_idx)) {
    printf("Error: getting frequency range for transmitter ch=%d. %s.\n", port_idx, device.get_error_message().c_str());
    return false;
  }

  // Check gain range.
  if (frequency.center_frequency_hz > range.stop() || frequency.center_frequency_hz < range.start()) {
    printf("Error: transmitter %d frequency %.2f MHz is out-of-range (%.2f to %.2f MHz)\n",
           port_idx,
           frequency.center_frequency_hz,
           range.start() / 1e6,
           range.stop() / 1e6);
    return false;
  }

  // Setup frequency.
  if (!device.set_tx_freq(port_idx, frequency)) {
    printf("Error: setting frequency for transmitter %d. %s.\n", port_idx, device.get_error_message().c_str());
  }

  return true;
}

bool radio_session_uhd_impl::set_rx_freq(unsigned int port_idx, radio_configuration::lo_frequency frequency)
{
  if (port_idx >= rx_port_map.size()) {
    printf("Error: receive port index (%d) exceeds the number of ports (%d).\n", port_idx, (int)tx_port_map.size());
    return false;
  }

  uhd::freq_range_t range;
  if (!device.get_rx_freq_range(range, port_idx)) {
    printf("Error: getting frequency range for receiver ch=%d. %s.\n", port_idx, device.get_error_message().c_str());
    return false;
  }

  // Check gain range.
  if (frequency.center_frequency_hz > range.stop() || frequency.center_frequency_hz < range.start()) {
    printf("Error: receiver %d frequency %.2f MHz is out-of-range (%.2f to %.2f MHz)\n",
           port_idx,
           frequency.center_frequency_hz,
           range.start() / 1e6,
           range.stop() / 1e6);
    return false;
  }

  // Setup frequency.
  if (!device.set_rx_freq(port_idx, frequency)) {
    printf("Error: setting frequency for receiver %d. %s.\n", port_idx, device.get_error_message().c_str());
    return false;
  }

  return true;
}

bool radio_session_uhd_impl::start_rx_stream()
{
  // Prevent multiple threads from starting streams simultaneously.
  std::unique_lock<std::mutex> lock(stream_start_mutex);

  if (!stream_start_required) {
    return true;
  }

  // Flag stream start is no longer required.
  stream_start_required = false;

  // Immediate start of the stream.
  uhd::time_spec_t time_spec = {};

  // Get current USRP time as timestamp.
  if (!device.get_time_now(time_spec)) {
    printf("Error: getting time to start stream. %s.\n", device.get_error_message().c_str());
    return false;
  }
  // Add delay to current time.
  time_spec += START_STREAM_DELAY_S;

  // Issue all streams to start.
  for (auto& stream : rx_streams) {
    if (!stream->start(time_spec)) {
      return false;
    }
  }

  return true;
}

radio_session_uhd_impl::radio_session_uhd_impl(const radio_configuration::radio& radio_config,
                                               task_executor&                    async_executor,
                                               radio_notification_handler&       notifier_) :
  device(async_executor, notifier_), sampling_rate_hz(radio_config.sampling_rate_hz)
{
  // Disable fast-path (U/L/O) messages.
  setenv("UHD_LOG_FASTPATH_DISABLE", "1", 0);

  // Set real time priority to UHD threads. All threads created from this thread inherit the priority.
  if (uhd_set_thread_priority(uhd_default_thread_priority, true) != UHD_ERROR_NONE) {
    Warning("Failed to set UHD RT thread priority.");
  }

  // Parse args into dictionary.
  uhd::device_addr_t device_addr(radio_config.args);

  // Set the logging level.
#ifdef UHD_LOG_INFO
  uhd::log::severity_level severity_level = uhd::log::severity_level::info;
  if (!radio_config.log_level.empty()) {
    std::string log_level = radio_config.log_level;

    for (auto& e : log_level) {
      e = std::toupper(e);
    }

    if (log_level == "WARNING") {
      severity_level = uhd::log::severity_level::warning;
    } else if (log_level == "INFO") {
      severity_level = uhd::log::severity_level::info;
    } else if (log_level == "DEBUG") {
      severity_level = uhd::log::severity_level::debug;
    } else if (log_level == "TRACE") {
      severity_level = uhd::log::severity_level::trace;
    } else {
      severity_level = uhd::log::severity_level::error;
    }
  }
  uhd::log::set_console_level(severity_level);
#endif

  // If device type or name not given in args, select device from found list.
  if (not device_addr.has_key("type")) {
    // Find available devices.
    uhd::device_addrs_t devices = uhd::device::find(uhd::device_addr_t());

    // Stop if no device is found.
    if (devices.empty()) {
      printf("Error: no radio devices found.\n");
      return;
    }

    // Select the first available device.
    uhd::device_addr_t first_device_addr = devices.front();

    // Append to the device address the device type.
    if (first_device_addr.has_key("type")) {
      device_addr.set("type", first_device_addr.get("type"));
    }
  }

  // Parse/Select master clock rate
  if (not device_addr.has_key("master_clock_rate") and device_addr.has_key("type")) {
    // Default master clock rate for B200 series
    std::string mcr;
    if (std::abs(std::remainder(23.04e6, radio_config.sampling_rate_hz)) < 1.0) {
      mcr = "23.04e6";
    } else if (std::abs(std::remainder(23.04e6, radio_config.sampling_rate_hz)) < 1.0) {
      mcr = "30.72e6";
    } else if (radio_config.sampling_rate_hz < 11.52e6) {
      mcr = std::to_string(radio_config.sampling_rate_hz * 4);
    } else {
      mcr = std::to_string(radio_config.sampling_rate_hz);
    }

    if (device_addr["type"] == "x300") {
      mcr = "184.32e6";
    } else if (device_addr["type"] == "n3xx") {
      mcr = "122.88e6";
    } else if (device_addr["type"] == "e3x0") {
      mcr = "30.72e6";
    }

    device_addr.set("master_clock_rate", mcr);
  }

  unsigned total_rx_channel_count = 0;
  for (const radio_configuration::stream& stream_config : radio_config.rx_streams) {
    total_rx_channel_count += stream_config.channels.size();
  }

  unsigned total_tx_channel_count = 0;
  for (const radio_configuration::stream& stream_config : radio_config.rx_streams) {
    total_tx_channel_count += stream_config.channels.size();
  }

  // Create UHD handler
  printf("Opening USRP tx_channels=%d, rx_channels=%d, args: %s\n",
         total_tx_channel_count,
         total_rx_channel_count,
         device_addr.to_string().c_str());

  // Make USRP
  if (!device.usrp_make(device_addr)) {
    printf("Error: opening device. %s.\n", device.get_error_message().c_str());
    return;
  }

  // If device name is not set, get it from motherboard
  std::string mboard_name;
  if (!device.get_mboard_name(mboard_name)) {
    return;
  }
  type = radio_uhd_device_type(mboard_name);

  bool        is_locked = false;
  std::string sensor_name;

  // Set sync source.
  if (!device.set_sync_source(radio_config.clock)) {
    return;
  }

  // Set GPS time if GPSDO is selected.
  if (radio_config.clock.sync == radio_configuration::clock_sources::source::GPSDO) {
    set_time_to_gps_time();
  }

  // Select oscillator sensor name.
  if (radio_config.clock.sync == radio_configuration::clock_sources::source::GPSDO) {
    sensor_name = "gps_locked";
  } else {
    sensor_name = "ref_locked";
  }

  // Wait until external reference / GPS is locked.
  if (radio_config.clock.sync == radio_configuration::clock_sources::source::GPSDO ||
      radio_config.clock.sync == radio_configuration::clock_sources::source::EXTERNAL) {
    // blocks until clock source is locked
    bool not_error = wait_sensor_locked(is_locked, sensor_name, true, 300);
    // Print Not lock error if the return was successful, wait_sensor_locked prints the error before returning
    if (not is_locked and !not_error) {
      printf(
          "Could not lock reference clock source. Sensor: %s=%s\n", sensor_name.c_str(), is_locked ? "true" : "false");
    }
  }

  // Set default Tx/Rx rates.
  if (!device.set_tx_rate(radio_config.sampling_rate_hz)) {
    printf("Error: setting Tx sampling rate. %s.", device.get_error_message().c_str());
    return;
  }
  if (!device.set_rx_rate(radio_config.sampling_rate_hz)) {
    printf("Error: setting Rx sampling rate. %s.", device.get_error_message().c_str());
    return;
  }

  // Reset timestamps.
  if ((total_rx_channel_count > 1 || total_tx_channel_count > 1) &&
      radio_config.clock.sync != radio_configuration::clock_sources::source::GPSDO) {
    device.set_time_unknown_pps(uhd::time_spec_t());
  }

  // For each transmit stream, create stream and configure RF ports.
  for (unsigned stream_idx = 0; stream_idx != radio_config.tx_streams.size(); ++stream_idx) {
    // Select stream.
    const radio_configuration::stream& stream = radio_config.tx_streams[stream_idx];

    // Prepare stream description.
    radio_uhd_tx_stream::stream_description stream_description = {};
    stream_description.id                                      = stream_idx;
    stream_description.otw_format                              = radio_config.otw_format;
    stream_description.srate_hz                                = radio_config.sampling_rate_hz;
    stream_description.args                                    = stream.args;

    // Setup ports.
    for (unsigned channel_idx = 0; channel_idx != stream.channels.size(); ++channel_idx) {
      // Select the port index.
      unsigned port_idx = tx_port_map.size();

      // Set channel port.
      stream_description.ports.emplace_back(port_idx);

      // Save the stream and channel indexes for the port.
      tx_port_map.emplace_back(port_to_stream_channel(stream_idx, channel_idx));
    }

    // Create transmit stream.
    if (tx_streams.emplace_back(device.create_tx_stream(stream_description)) == nullptr) {
      return;
    }

    // Setup port.
    for (unsigned channel_idx = 0; channel_idx != stream.channels.size(); ++channel_idx) {
      // Get the port index.
      unsigned port_idx = stream_description.ports[channel_idx];

      // Extract port configuration.
      const radio_configuration::channel& channel = stream.channels[channel_idx];

      // Setup gain.
      set_tx_gain_unprotected(port_idx, channel.gain_dB);

      // Setup frequency.
      set_tx_freq(port_idx, channel.freq);

      // Inform about ignored argument.
      if (!channel.args.empty()) {
        printf("Warning: transmitter %d unused args.\n", port_idx);
      }
    }
  }

  // For each receive stream, create stream and configure RF ports.
  for (unsigned stream_idx = 0; stream_idx != radio_config.rx_streams.size(); ++stream_idx) {
    // Select stream.
    const radio_configuration::stream& stream = radio_config.rx_streams[stream_idx];

    // Prepare stream description.
    radio_uhd_rx_stream::stream_description stream_description = {};
    stream_description.id                                      = stream_idx;
    stream_description.otw_format                              = radio_config.otw_format;
    stream_description.args                                    = stream.args;

    // Setup ports.
    for (unsigned channel_idx = 0; channel_idx != stream.channels.size(); ++channel_idx) {
      // Select the port index.
      unsigned port_idx = rx_port_map.size();

      // Set channel port.
      stream_description.ports.emplace_back(port_idx);

      // Save the stream and channel indexes for the port.
      rx_port_map.emplace_back(port_to_stream_channel(stream_idx, channel_idx));
    }

    // Create receive stream.
    if (rx_streams.emplace_back(device.create_rx_stream(stream_description)) == nullptr) {
      return;
    }

    // Setup port.
    for (unsigned channel_idx = 0; channel_idx != stream.channels.size(); ++channel_idx) {
      // Get the port index.
      unsigned port_idx = stream_description.ports[channel_idx];

      // Extract port configuration.
      const radio_configuration::channel& channel = stream.channels[channel_idx];

      // Setup gain.
      if (!set_rx_gain_unprotected(port_idx, channel.gain_dB)) {
        return;
      }

      // Setup frequency.
      if (!set_rx_freq(port_idx, channel.freq)) {
        return;
      }

      // Inform about ignored argument.
      if (!channel.args.empty()) {
        printf("Warning: transmitter %d unused args.\n", port_idx);
      }
    }
  }

  // Restore thread priorities.
  if (uhd_set_thread_priority(0, false) != UHD_ERROR_NONE) {
    printf("Error: setting UHD thread priority.");
    return;
  }

  // Transition to successfully initialized.
  state = states::SUCCESSFUL_INIT;
}

void radio_session_uhd_impl::stop()
{
  // Transition state to stop.
  state = states::STOP;

  // Iterate transmit streams and stop.
  for (auto& stream : tx_streams) {
    stream->stop();
  }

  // Iterate receive streams and stop.
  for (auto& stream : rx_streams) {
    stream->stop();
  }
}

void radio_session_uhd_impl::transmit(unsigned int                                  stream_id,
                                      const baseband_gateway_transmitter::metadata& metadata,
                                      baseband_gateway_buffer&                      data)
{
  report_fatal_error_if_not(stream_id < tx_streams.size(),
                            "Stream identifier (%d) exceeds the number of transmit streams  (%d).",
                            stream_id,
                            (int)rx_streams.size());

  uhd::time_spec_t time_spec = time_spec.from_ticks(metadata.ts, sampling_rate_hz);
  tx_streams[stream_id]->transmit(data, time_spec);
}

// See interface for documentation.
baseband_gateway_receiver::metadata radio_session_uhd_impl::receive(baseband_gateway_buffer& data, unsigned stream_id)
{
  baseband_gateway_receiver::metadata ret = {};
  report_fatal_error_if_not(stream_id < rx_streams.size(),
                            "Stream identifier (%d) exceeds the number of receive streams  (%d).",
                            stream_id,
                            (int)rx_streams.size());

  if (!start_rx_stream()) {
    return ret;
  }

  uhd::time_spec_t time_spec = {};
  if (!rx_streams[stream_id]->receive(data, time_spec)) {
    return ret;
  }

  ret.ts = static_cast<baseband_gateway_timestamp>(time_spec.get_full_secs()) *
               static_cast<baseband_gateway_timestamp>(sampling_rate_hz) +
           static_cast<baseband_gateway_timestamp>(sampling_rate_hz * time_spec.get_frac_secs());

  return ret;
}

std::unique_ptr<radio_session> radio_factory_uhd_impl::create(const radio_configuration::radio& config,
                                                              task_executor&                    async_task_executor,
                                                              radio_notification_handler&       notifier)
{
  std::unique_ptr<radio_session_uhd_impl> device =
      std::make_unique<radio_session_uhd_impl>(config, async_task_executor, notifier);
  if (!device->is_successful()) {
    return nullptr;
  }

  return std::move(device);
}
