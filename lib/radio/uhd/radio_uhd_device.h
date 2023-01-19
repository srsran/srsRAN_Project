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

#include "radio_uhd_device_type.h"
#include "radio_uhd_exception_handler.h"
#include "radio_uhd_rx_stream.h"
#include "radio_uhd_tx_stream.h"
#include "srsgnb/radio/radio_session.h"

namespace srsgnb {

class radio_uhd_device : public uhd_exception_handler
{
private:
  uhd::usrp::multi_usrp::sptr usrp = nullptr;

public:
  struct properties {
    radio_uhd_device_type                                device_type;
    std::set<radio_configuration::clock_sources::source> supported_clock_sources = {
        radio_configuration::clock_sources::source::DEFAULT};
    std::set<radio_configuration::clock_sources::source> supported_sync_sources = {
        radio_configuration::clock_sources::source::DEFAULT};
    uhd::meta_range_t                                   tx_gain_range;
    uhd::meta_range_t                                   rx_gain_range;
    uhd::meta_range_t                                   tx_lo_freq_range;
    uhd::meta_range_t                                   rx_lo_freq_range;
    std::set<radio_configuration::over_the_wire_format> supported_otw_formats = {
        radio_configuration::over_the_wire_format::DEFAULT};
  };

  bool is_valid() const { return usrp != nullptr; }

  bool usrp_make(const std::string& device_address)
  {
    // Parse args into dictionary.
    uhd::device_addr_t device_addr(device_address);

    // Destroy any previous USRP instance
    usrp = nullptr;

    // If device type or name not given in args, select device from found list.
    if (not device_addr.has_key("type")) {
      // Find available devices that match the given device address.
      uhd::device_addrs_t devices = uhd::device::find(device_addr);

      // Stop if no device is found.
      if (devices.empty()) {
        printf("Error: no radio devices found.\n");
        return false;
      }

      // Select the first available device.
      uhd::device_addr_t first_device_addr = devices.front();
      Info("No device type given, found device with address '" << first_device_addr.to_string() << "'");

      // Append to the device address the device type.
      if (first_device_addr.has_key("type")) {
        device_addr.set("type", first_device_addr.get("type"));
      }
    }

    // If device type is known, parse and select default address parameters.
    if (device_addr.has_key("type")) {
      // Get the device type.
      radio_uhd_device_type device_type(device_addr.get("type"));

      // If the device is X300.
      if (device_type == radio_uhd_device_type::types::X300) {
        // Set the default master clock rate.
        if (!device_addr.has_key("master_clock_rate")) {
          device_addr.set("master_clock_rate", "184.32e6");
        }
        // Set the default send frame size.
        if (!device_addr.has_key("send_frame_size")) {
          device_addr.set("send_frame_size", "8000");
        }
        // Set the default receive frame size.
        if (!device_addr.has_key("recv_frame_size")) {
          device_addr.set("recv_frame_size", "8000");
        }
      } else if (device_type == radio_uhd_device_type::types::N300) {
        // Set the default master clock rate.
        if (!device_addr.has_key("master_clock_rate")) {
          device_addr.set("master_clock_rate", "122.88e6");
        }
      }
    }

    Info("Making USRP object with args '" << device_addr.to_string() << "'");

    return safe_execution([this, &device_addr]() { usrp = uhd::usrp::multi_usrp::make(device_addr); });
  }

  bool get_properties(properties& props)
  {
    return safe_execution([this, &props]() {
      uhd::fs_path DEVICE_NAME_PATH("/mboards/0/name");
      uhd::fs_path CLOCK_SOURCES_PATH("/mboards/0/clock_source/options");
      uhd::fs_path SYNC_SOURCES_PATH("/mboards/0/time_source/options");
      uhd::fs_path TX_GAIN_PATH("/mboards/0/dboards/A/tx_frontends/0/gains/PGA0/range");
      uhd::fs_path RX_GAIN_PATH("/mboards/0/dboards/A/rx_frontends/0/gains/PGA0/range");
      uhd::fs_path TX_LO_FREQ_PATH("/mboards/0/dboards/A/tx_frontends/0/freq/range");
      uhd::fs_path RX_LO_FREQ_PATH("/mboards/0/dboards/A/rx_frontends/0/freq/range");

      // Get device tree
      uhd::property_tree::sptr tree = usrp->get_device()->get_tree();

      // Get device name.
      std::string device_name = "unknown";
      if (tree->exists(DEVICE_NAME_PATH)) {
        device_name = tree->access<std::string>(DEVICE_NAME_PATH).get();
      }
      props.device_type = radio_uhd_device_type(device_name);

      // Parse Clock sources
      if (tree->exists(CLOCK_SOURCES_PATH)) {
        const std::vector<std::string> clock_sources = tree->access<std::vector<std::string>>(CLOCK_SOURCES_PATH).get();
        for (const std::string& source : clock_sources) {
          if (source == "internal") {
            props.supported_clock_sources.emplace(radio_configuration::clock_sources::source::INTERNAL);
          } else if (source == "external") {
            props.supported_clock_sources.emplace(radio_configuration::clock_sources::source::EXTERNAL);
          } else if (source == "gpsdo") {
            props.supported_clock_sources.emplace(radio_configuration::clock_sources::source::GPSDO);
          }
        }
      }

      // Parse Sync sources.
      if (tree->exists(SYNC_SOURCES_PATH)) {
        const std::vector<std::string> time_sources = tree->access<std::vector<std::string>>(SYNC_SOURCES_PATH).get();
        for (const std::string& source : time_sources) {
          if (source == "internal") {
            props.supported_sync_sources.emplace(radio_configuration::clock_sources::source::INTERNAL);
          } else if (source == "external") {
            props.supported_sync_sources.emplace(radio_configuration::clock_sources::source::EXTERNAL);
          } else if (source == "gpsdo") {
            props.supported_sync_sources.emplace(radio_configuration::clock_sources::source::GPSDO);
          }
        }
      }

      // Get Tx gain range.
      if (tree->exists(TX_GAIN_PATH)) {
        props.tx_gain_range = tree->access<uhd::meta_range_t>(TX_GAIN_PATH).get();
      }

      // Get Rx gain range.
      if (tree->exists(RX_GAIN_PATH)) {
        props.rx_gain_range = tree->access<uhd::meta_range_t>(RX_GAIN_PATH).get();
      }

      // Get Tx LO frequency range.
      if (tree->exists(RX_GAIN_PATH)) {
        props.tx_lo_freq_range = tree->access<uhd::meta_range_t>(TX_LO_FREQ_PATH).get();
      }

      // Get Rx LO frequency range.
      if (tree->exists(RX_GAIN_PATH)) {
        props.rx_lo_freq_range = tree->access<uhd::meta_range_t>(RX_LO_FREQ_PATH).get();
      }

      // Parse over-the-wire formats.
      switch (props.device_type) {
        case radio_uhd_device_type::types::X300:
          props.supported_otw_formats.emplace(radio_configuration::over_the_wire_format::SC16);
          break;
        case radio_uhd_device_type::types::B200:
          props.supported_otw_formats.emplace(radio_configuration::over_the_wire_format::SC16);
          props.supported_otw_formats.emplace(radio_configuration::over_the_wire_format::SC12);
          break;
        case radio_uhd_device_type::types::N300:
        case radio_uhd_device_type::types::E3X0:
        case radio_uhd_device_type::types::UNKNOWN:
        default:
          break;
      }
    });
  }
  bool get_mboard_sensor_names(std::vector<std::string>& sensors)
  {
    return safe_execution([this, &sensors]() { sensors = usrp->get_mboard_sensor_names(); });
  }
  bool get_rx_sensor_names(std::vector<std::string>& sensors)
  {
    return safe_execution([this, &sensors]() { sensors = usrp->get_rx_sensor_names(); });
  }
  bool get_sensor(const std::string& sensor_name, double& sensor_value)
  {
    return safe_execution(
        [this, &sensor_name, &sensor_value]() { sensor_value = usrp->get_mboard_sensor(sensor_name).to_real(); });
  }
  bool get_sensor(const std::string& sensor_name, bool& sensor_value)
  {
    return safe_execution(
        [this, &sensor_name, &sensor_value]() { sensor_value = usrp->get_mboard_sensor(sensor_name).to_bool(); });
  }
  bool get_rx_sensor(const std::string& sensor_name, bool& sensor_value)
  {
    return safe_execution(
        [this, &sensor_name, &sensor_value]() { sensor_value = usrp->get_rx_sensor(sensor_name).to_bool(); });
  }
  bool set_time_unknown_pps(const uhd::time_spec_t& timespec)
  {
    return safe_execution([this, &timespec]() { usrp->set_time_unknown_pps(timespec); });
  }
  bool set_master_clock_rate(double mcr)
  {
    return safe_execution([this, &mcr]() { usrp->set_master_clock_rate(mcr); });
  }
  bool get_time_now(uhd::time_spec_t& timespec)
  {
    return safe_execution([this, &timespec]() { timespec = usrp->get_time_now(); });
  }
  bool set_sync_source(const radio_configuration::clock_sources& config)
  {
    // Convert clock source to string.
    std::string clock_src;
    switch (config.clock) {
      case radio_configuration::clock_sources::source::DEFAULT:
      case radio_configuration::clock_sources::source::INTERNAL:
        clock_src = "internal";
        break;
      case radio_configuration::clock_sources::source::EXTERNAL:
        clock_src = "external";
        break;
      case radio_configuration::clock_sources::source::GPSDO:
        clock_src = "gpsdo";
        break;
    }

    // Convert sync source to string.
    std::string sync_src;
    switch (config.sync) {
      case radio_configuration::clock_sources::source::DEFAULT:
      case radio_configuration::clock_sources::source::INTERNAL:
        sync_src = "internal";
        break;
      case radio_configuration::clock_sources::source::EXTERNAL:
        sync_src = "external";
        break;
      case radio_configuration::clock_sources::source::GPSDO:
        sync_src = "gpsdo";
        break;
    }

    Debug("Setting PPS source to '" << sync_src << "' and clock source to '" << clock_src << "'");
#if UHD_VERSION < 3140099
    return safe_execution([this, &sync_src, &clock_src]() {
      usrp->set_time_source(sync_src);
      usrp->set_clock_source(clock_src);
    });
#else
    return safe_execution([this, &sync_source, &clock_source]() { usrp->set_sync_source(clock_source, sync_source); });
#endif
  }
  bool get_tx_gain_range(uhd::gain_range_t& range, unsigned ch)
  {
    return safe_execution([this, &range, ch]() { range = usrp->get_tx_gain_range(ch); });
  }
  bool get_rx_gain_range(uhd::gain_range_t& range, unsigned ch)
  {
    return safe_execution([this, &range, ch]() { range = usrp->get_rx_gain_range(ch); });
  }
  bool get_tx_freq_range(uhd::freq_range_t& range, unsigned ch)
  {
    return safe_execution([this, &range, ch]() { range = usrp->get_tx_freq_range(ch); });
  }
  bool get_rx_freq_range(uhd::gain_range_t& range, unsigned ch)
  {
    return safe_execution([this, &range, ch]() { range = usrp->get_rx_freq_range(ch); });
  }
  bool set_rx_rate(double rate)
  {
    Debug("Setting Rx Rate to " << rate / 1e6 << "MHz");
    return safe_execution([this, rate]() { usrp->set_rx_rate(rate); });
  }
  bool set_tx_rate(double rate)
  {
    Debug("Setting Tx Rate to " << rate / 1e6 << "MHz");
    return safe_execution([this, rate]() { usrp->set_tx_rate(rate); });
  }
  bool set_command_time(const uhd::time_spec_t& timespec)
  {
    return safe_execution([this, &timespec]() { usrp->set_command_time(timespec); });
  }
  std::unique_ptr<radio_uhd_tx_stream> create_tx_stream(task_executor&                                 async_executor,
                                                        radio_notification_handler&                    notifier,
                                                        const radio_uhd_tx_stream::stream_description& description)
  {
    std::unique_ptr<radio_uhd_tx_stream> stream =
        std::make_unique<radio_uhd_tx_stream>(usrp, description, async_executor, notifier);

    if (stream->is_successful()) {
      return stream;
    }

    return nullptr;
  }
  std::unique_ptr<radio_uhd_rx_stream> create_rx_stream(radio_notification_handler&                    notifier,
                                                        const radio_uhd_rx_stream::stream_description& description)
  {
    std::unique_ptr<radio_uhd_rx_stream> stream = std::make_unique<radio_uhd_rx_stream>(usrp, description, notifier);

    if (stream->is_successful()) {
      return stream;
    }

    printf("Error: failed to create receive stream %d. %s.", description.id, stream->get_error_message().c_str());
    return nullptr;
  }
  bool set_tx_gain(unsigned ch, double gain)
  {
    Debug("Setting channel " << ch << " Tx gain to " << gain << " dB");

    return safe_execution([this, ch, gain]() { usrp->set_tx_gain(gain, ch); });
  }
  bool set_rx_gain(size_t ch, double gain)
  {
    Debug("Setting channel " << ch << " Rx gain to " << gain << " dB");

    return safe_execution([this, ch, gain]() { usrp->set_rx_gain(gain, ch); });
  }
  bool set_tx_freq(uint32_t ch, const radio_configuration::lo_frequency& config)
  {
    Debug("Setting channel " << ch << " Tx frequency to " << config.center_frequency_hz / 1e6 << " MHz");

    // Create tune request.
    uhd::tune_request_t tune_request(config.center_frequency_hz);

    // If the LO frequency is defined, force a LO frequency and set the DSP frequency to auto.
    if (std::isnormal(config.lo_frequency_hz)) {
      tune_request.rf_freq         = config.lo_frequency_hz;
      tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
      tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_AUTO;
    }

    return safe_execution([this, ch, &tune_request]() { usrp->set_tx_freq(tune_request, ch); });
  }
  bool set_rx_freq(uint32_t ch, const radio_configuration::lo_frequency& config)
  {
    Debug("Setting channel " << ch << " Rx frequency to " << config.center_frequency_hz / 1e6 << " MHz");

    // Create tune request.
    uhd::tune_request_t tune_request(config.center_frequency_hz);

    // If the LO frequency is defined, force a LO frequency and set the DSP frequency to auto.
    if (std::isnormal(config.lo_frequency_hz)) {
      tune_request.rf_freq         = config.lo_frequency_hz;
      tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
      tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_AUTO;
    }

    return safe_execution([this, ch, &tune_request]() { usrp->set_rx_freq(tune_request, ch); });
  }
};

} // namespace srsgnb
