
#ifndef SRSGNB_LIB_RADIO_UHD_RADIO_UHD_DEVICE_H
#define SRSGNB_LIB_RADIO_UHD_RADIO_UHD_DEVICE_H

#include "radio_uhd_exception_handler.h"
#include "radio_uhd_rx_stream.h"
#include "radio_uhd_tx_stream.h"
#include "srsgnb/radio/radio_session.h"

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__
#include <uhd/usrp/multi_usrp.hpp>
#pragma GCC diagnostic pop

namespace srsgnb {

class radio_uhd_device : public uhd_exception_handler
{
private:
  uhd::usrp::multi_usrp::sptr usrp = nullptr;
  task_executor&              async_executor;
  radio_notification_handler&             notifier;

public:
  radio_uhd_device(task_executor& async_executor_, radio_notification_handler& notifier_) :
    async_executor(async_executor_), notifier(notifier_)
  {
    // Do nothing.
  }

  bool usrp_make(const uhd::device_addr_t& dev_addr)
  {
    // Destroy any previous USRP instance
    usrp = nullptr;

    Debug("Making USRP object with args '" << dev_addr.to_string() << "'");

    return safe_execution([this, &dev_addr]() { usrp = uhd::usrp::multi_usrp::make(dev_addr); });
  }
  bool get_mboard_name(std::string& mboard_name)
  {
    return safe_execution([this, &mboard_name]() { mboard_name = usrp->get_mboard_name(); });
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
      usrp->set_time_source(clock_src);
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
  std::unique_ptr<radio_uhd_tx_stream> create_tx_stream(const radio_uhd_tx_stream::stream_description& description)
  {
    std::unique_ptr<radio_uhd_tx_stream> stream =
        std::make_unique<radio_uhd_tx_stream>(usrp, description, async_executor, notifier);

    if (stream->is_successful()) {
      return stream;
    }

    return nullptr;
  }
  std::unique_ptr<radio_uhd_rx_stream> create_rx_stream(const radio_uhd_rx_stream::stream_description& description)
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
#endif // SRSGNB_LIB_RADIO_UHD_RADIO_UHD_DEVICE_H
