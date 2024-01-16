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

#pragma once

#include "radio_uhd_device_type.h"
#include "radio_uhd_exception_handler.h"
#include "radio_uhd_rx_stream.h"
#include "radio_uhd_tx_stream.h"
#include "srsran/radio/radio_session.h"

/// \brief Determines whether a frequency is valid within a range.
///
/// A frequency is considered valid within a range if the range clips the frequency value within 1 Hz error.
static bool radio_uhd_device_validate_freq_range(const uhd::freq_range_t& range, double freq)
{
  double clipped_freq = range.clip(freq);
  return std::abs(clipped_freq - freq) < 1.0;
}

/// \brief Determines whether a gain is valid within a range.
///
/// A gain is considered valid within a range if the range clips the frequency value within 0.01 error.
static bool radio_uhd_device_validate_gain_range(const uhd::gain_range_t& range, double gain)
{
  int64_t clipped_gain = static_cast<uint64_t>(std::round(range.clip(gain, true) * 100));
  int64_t uint_gain    = static_cast<uint64_t>(gain * 100);

  return (clipped_gain == uint_gain);
}

static double to_MHz(double value_Hz)
{
  return value_Hz * 1e-6;
}

namespace srsran {

class radio_uhd_device : public uhd_exception_handler
{
public:
  radio_uhd_device() : logger(srslog::fetch_basic_logger("RF")) {}

  bool is_valid() const { return usrp != nullptr; }

  bool usrp_make(const std::string& device_address)
  {
    // Parse args into dictionary.
    uhd::device_addr_t device_addr;
    if (!safe_execution([device_address, &device_addr]() { device_addr = uhd::device_addr_t(device_address); })) {
      return false;
    }

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
      fmt::print("No device type given, found device with address '{}'\n", first_device_addr.to_string());

      // Append to the device address the device type.
      if (first_device_addr.has_key("type")) {
        device_addr.set("type", first_device_addr.get("type"));
      }
    }

    // If device type is known, parse and select default address parameters.
    if (device_addr.has_key("type")) {
      // Get the device type.
      type = radio_uhd_device_type(device_addr.get("type"));

      switch (type) {
        case radio_uhd_device_type::types::E3xx:
          // Set the default master clock rate.
          if (!device_addr.has_key("master_clock_rate")) {
            device_addr.set("master_clock_rate", "30.72e6");
          }
          break;
        case radio_uhd_device_type::types::X3x0:
          // Set the default master clock rate.
          if (!device_addr.has_key("master_clock_rate")) {
            device_addr.set("master_clock_rate", "184.32e6");
          }
          break;
        case radio_uhd_device_type::types::X410:
          // Set the default master clock rate.
          if (!device_addr.has_key("master_clock_rate")) {
            device_addr.set("master_clock_rate", "245.76e6");
          }
          break;
        case radio_uhd_device_type::types::N3x0:
          // Set the default master clock rate.
          if (!device_addr.has_key("master_clock_rate")) {
            device_addr.set("master_clock_rate", "122.88e6");
          }
          break;
        case radio_uhd_device_type::types::N32x:
          // Set the default master clock rate.
          if (!device_addr.has_key("master_clock_rate")) {
            device_addr.set("master_clock_rate", "245.76e6");
          }
          break;
        case radio_uhd_device_type::types::B2xx:
        case radio_uhd_device_type::types::UNKNOWN:
        default:
          // No default parameters are required.
          break;
      }

      // Set frame sizes for network based USRP devices.
      if ((type == radio_uhd_device_type::types::N32x) || (type == radio_uhd_device_type::types::N3x0) ||
          (type == radio_uhd_device_type::types::X3x0) || (type == radio_uhd_device_type::types::X410)) {
        // Set the default send frame size.
        if (!device_addr.has_key("send_frame_size")) {
          device_addr.set("send_frame_size", "8000");
        }
        // Set the default receive frame size.
        if (!device_addr.has_key("recv_frame_size")) {
          device_addr.set("recv_frame_size", "8000");
        }
      }
    }

    fmt::print("Making USRP object with args '{}'\n", device_addr.to_string());

    return safe_execution([this, &device_addr]() { usrp = uhd::usrp::multi_usrp::make(device_addr); });
  }
  bool is_connection_valid()
  {
    // If the device is a B2xx, check if the USB is version 3.
    if (type == radio_uhd_device_type::types::B2xx) {
      // Get USB version.
      unsigned usb_version = 0;
      bool     success     = safe_execution([this, &usb_version]() {
        usb_version = usrp->get_device()->get_tree()->access<unsigned>("/mboards/0/usb_version").get();
      });

      // The USB version is invalid if the USB version is accessed successfully and the version is not 3.
      if (success && (usb_version != 3)) {
        fmt::print("USRP operating over USB version {}, USB 3 is required.\n", usb_version);
        return false;
      }
    }

    return true;
  }
  radio_uhd_device_type get_type() const { return type; }
  bool                  get_mboard_sensor_names(std::vector<std::string>& sensors)
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
  bool set_automatic_master_clock_rate(double srate_Hz)
  {
    return safe_execution([this, &srate_Hz]() {
      // Get range of valid master clock rates.
      uhd::meta_range_t range = usrp->get_master_clock_rate_range();

      // Select the nearest valid master clock rate.
      double mcr_Hz = range.clip(srate_Hz);

      usrp->set_master_clock_rate(mcr_Hz);
    });
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

    logger.debug("Setting PPS source to '{}' and clock source to '{}'.", sync_src, clock_src);
#if UHD_VERSION < 3140099
    return safe_execution([this, &sync_src, &clock_src]() {
      std::vector<std::string> time_sources = usrp->get_time_sources(0);
      if (std::find(time_sources.begin(), time_sources.end(), sync_src) == time_sources.end()) {
        on_error("Invalid time source {}. Supported: {}", sync_src, span<const std::string>(time_sources));
        return;
      }
      std::vector<std::string> clock_sources = usrp->get_clock_sources(0);
      if (std::find(clock_sources.begin(), clock_sources.end(), clock_src) == clock_sources.end()) {
        on_error("Invalid clock source {}. Supported: {}", clock_src, span<const std::string>(clock_sources));
        return;
      }

      usrp->set_time_source(sync_src);
      usrp->set_clock_source(clock_src);
    });
#else
    return safe_execution([this, &sync_source, &clock_source]() { usrp->set_sync_source(clock_source, sync_source); });
#endif
  }
  bool set_rx_rate(double& actual_rate, double rate)
  {
    logger.debug("Setting Rx Rate to {} MHz.", to_MHz(rate));

    return safe_execution([this, &actual_rate, rate]() {
      uhd::meta_range_t range = usrp->get_rx_rates();

      if (!radio_uhd_device_validate_freq_range(range, rate)) {
        on_error("Rx Rate {} MHz is invalid. The nearest valid value is {}.", to_MHz(rate), to_MHz(range.clip(rate)));
        return;
      }

      usrp->set_rx_rate(rate);

      actual_rate = usrp->get_rx_rate();
    });
  }
  bool set_tx_rate(double& actual_rate, double rate)
  {
    logger.debug("Setting Tx Rate to {} MHz.", to_MHz(rate));

    return safe_execution([this, &actual_rate, rate]() {
      uhd::meta_range_t range = usrp->get_tx_rates();

      if (!radio_uhd_device_validate_freq_range(range, rate)) {
        on_error("Tx Rate {} MHz is invalid. The nearest valid value is {}.", to_MHz(rate), to_MHz(range.clip(rate)));
        return;
      }

      usrp->set_tx_rate(rate);

      actual_rate = usrp->get_tx_rate();
    });
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
    logger.debug("Setting channel {} Tx gain to {:.2f} dB.", ch, gain);

    return safe_execution([this, ch, gain]() {
      uhd::gain_range_t range = usrp->get_tx_gain_range(ch);

      if (!radio_uhd_device_validate_gain_range(range, gain)) {
        on_error("Tx gain (i.e., {} dB) is out-of-range. Range is [{}, {}] dB in steps of {} dB.",
                 gain,
                 range.start(),
                 range.stop(),
                 range.step());
        return;
      }

      usrp->set_tx_gain(gain, ch);
    });
  }
  bool set_rx_gain(size_t ch, double gain)
  {
    logger.debug("Setting channel {} Rx gain to {:.2f} dB.", ch, gain);

    return safe_execution([this, ch, gain]() {
      uhd::gain_range_t range = usrp->get_rx_gain_range(ch);

      if (!radio_uhd_device_validate_gain_range(range, gain)) {
        on_error("Rx gain (i.e., {} dB) is out-of-range. Range is [{}, {}] dB in steps of {} dB.",
                 gain,
                 range.start(),
                 range.stop(),
                 range.step());
        return;
      }

      usrp->set_rx_gain(gain, ch);
    });
  }
  bool set_tx_freq(uint32_t ch, const radio_configuration::lo_frequency& config)
  {
    logger.debug("Setting channel {} Tx frequency to {} MHz.", ch, to_MHz(config.center_frequency_hz));

    return safe_execution([this, ch, &config]() {
      uhd::freq_range_t range = usrp->get_tx_freq_range(ch);

      if (!radio_uhd_device_validate_freq_range(range, config.center_frequency_hz)) {
        on_error("Tx RF frequency {} MHz is out-of-range. Range is {}.",
                 to_MHz(config.center_frequency_hz),
                 range.to_pp_string());
        return;
      }

      // Create tune request.
      uhd::tune_request_t tune_request(config.center_frequency_hz);

      // If the LO frequency is defined, force a LO frequency and set the DSP frequency to auto.
      if (std::isnormal(config.lo_frequency_hz)) {
        tune_request.rf_freq         = config.lo_frequency_hz;
        tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
        tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_AUTO;
      }

      usrp->set_tx_freq(tune_request, ch);
    });
  }
  bool set_rx_freq(uint32_t ch, const radio_configuration::lo_frequency& config)
  {
    logger.debug("Setting channel {} Rx frequency to {} MHz.", ch, to_MHz(config.center_frequency_hz));

    return safe_execution([this, ch, &config]() {
      uhd::freq_range_t range = usrp->get_rx_freq_range(ch);

      if (!radio_uhd_device_validate_freq_range(range, config.center_frequency_hz)) {
        on_error("Rx RF frequency {} MHz is out-of-range. Range is {}.",
                 to_MHz(config.center_frequency_hz),
                 range.to_pp_string());
        return;
      }

      // Create tune request.
      uhd::tune_request_t tune_request(config.center_frequency_hz);

      // If the LO frequency is defined, force a LO frequency and set the DSP frequency to auto.
      if (std::isnormal(config.lo_frequency_hz)) {
        tune_request.rf_freq         = config.lo_frequency_hz;
        tune_request.rf_freq_policy  = uhd::tune_request_t::POLICY_MANUAL;
        tune_request.dsp_freq_policy = uhd::tune_request_t::POLICY_AUTO;
      }

      usrp->set_rx_freq(tune_request, ch);
    });
  }

private:
  uhd::usrp::multi_usrp::sptr usrp = nullptr;
  radio_uhd_device_type       type = radio_uhd_device_type::types::UNKNOWN;
  srslog::basic_logger&       logger;
};

} // namespace srsran
