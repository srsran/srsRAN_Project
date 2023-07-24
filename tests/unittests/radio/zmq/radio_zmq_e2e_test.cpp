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

#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/srsran_test.h"
#include <random>
#include <unistd.h>

using namespace srsran;

/// Describes a test parameters.
struct test_description {
  /// Indicates the number of streams.
  unsigned nof_streams;
  /// Indicates the number of channels per stream.
  unsigned nof_channels;
  /// Indicates the transmit center frequency for all ports.
  double tx_freq_hz;
  /// Indicates the transmit gain for all ports in decibels.
  double tx_gain_db;
  /// Provides a list for all the transmit ports addresses.
  std::vector<std::string> tx_addresses;
  /// Sampling rate in hertz.
  double sample_rate_hz;
  /// Indicates the receive center frequency for all ports.
  double rx_freq_hz;
  /// Indicates the receive gain for all ports in decibels.
  double rx_gain_db;
  /// Provides a list for all the receive ports addresses.
  std::vector<std::string> rx_addresses;
  /// Indicates the radio logging level.
  std::string log_level;
  /// Provides the number of samples per transmission.
  unsigned block_size;
  /// Number of blocks.
  unsigned nof_blocks;
};

class radio_notifier_spy : public radio_notification_handler
{
public:
  void on_radio_rt_event(const event_description& description) override {}
};

static constexpr float                       ASSERT_MAX_ERROR = 1e-6;
static unsigned                              seed             = 1234;
static std::mt19937                          tx_rgen(seed);
static std::mt19937                          rx_rgen(seed);
static std::uniform_real_distribution<float> dist(-1.0, +1.0);

static void test(const test_description& config, radio_factory& factory, task_executor& async_task_executor)
{
  // Number of addresses must be coherent with the total number of ports.
  TESTASSERT_EQ(config.nof_channels * config.nof_streams, config.tx_addresses.size());
  TESTASSERT_EQ(config.nof_channels * config.nof_streams, config.rx_addresses.size());

  // Prepare radio configuration.
  radio_configuration::radio radio_config;
  for (unsigned stream_id = 0, port_index = 0; stream_id != config.nof_streams; ++stream_id) {
    radio_configuration::stream stream_config;
    for (unsigned channel_id = 0; channel_id != config.nof_channels; ++channel_id, ++port_index) {
      radio_configuration::channel channel_config;
      channel_config.freq.center_frequency_hz = config.tx_freq_hz;
      channel_config.gain_dB                  = config.tx_gain_db;
      channel_config.args                     = config.tx_addresses[port_index];
      stream_config.channels.push_back(channel_config);
    }
    radio_config.tx_streams.push_back(stream_config);
  }
  for (unsigned stream_id = 0, port_index = 0; stream_id != config.nof_streams; ++stream_id) {
    radio_configuration::stream stream_config;
    for (unsigned channel_id = 0; channel_id != config.nof_channels; ++channel_id, ++port_index) {
      radio_configuration::channel channel_config;
      channel_config.freq.center_frequency_hz = config.rx_freq_hz;
      channel_config.gain_dB                  = config.rx_gain_db;
      channel_config.args                     = config.rx_addresses[port_index];
      stream_config.channels.push_back(channel_config);
    }
    radio_config.rx_streams.push_back(stream_config);
  }
  radio_config.log_level        = config.log_level;
  radio_config.sampling_rate_hz = config.sample_rate_hz;

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Create radio session.
  std::unique_ptr<radio_session> session = factory.create(radio_config, async_task_executor, radio_notifier);
  TESTASSERT(session, "Failed to open radio session.");

  // Calculate starting time.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = session->read_current_time();
  baseband_gateway_timestamp start_time = current_time + static_cast<uint64_t>(delay_s * radio_config.sampling_rate_hz);

  // Start processing.
  session->start(start_time);

  // Prepare transmit buffer
  baseband_gateway_buffer_dynamic tx_buffer(config.nof_channels, config.block_size);

  // Prepare receive buffer.
  baseband_gateway_buffer_dynamic rx_buffer(config.nof_channels, config.block_size);

  for (unsigned block_id = 0; block_id != config.nof_blocks; ++block_id) {
    // Transmit for each stream the same buffer.
    for (unsigned stream_id = 0; stream_id != config.nof_streams; ++stream_id) {
      // Get baseband gateway.
      baseband_gateway& bb_gateway = session->get_baseband_gateway(stream_id);

      // Select transmitter.
      baseband_gateway_transmitter& transmitter = bb_gateway.get_transmitter();

      // Generate transmit random data for each channel.
      for (unsigned channel_id = 0; channel_id != config.nof_channels; ++channel_id) {
        span<cf_t> buffer = tx_buffer[channel_id];
        for (cf_t& sample : buffer) {
          sample = {dist(tx_rgen), dist(tx_rgen)};
        }
      }

      // Transmit stream buffer.
      baseband_gateway_transmitter::metadata tx_md;
      tx_md.ts = start_time + block_id * config.block_size;
      transmitter.transmit(tx_buffer.get_reader(), tx_md);
    }

    // Receive for each stream the same buffer.
    for (unsigned stream_id = 0; stream_id != config.nof_streams; ++stream_id) {
      // Get baseband gateway.
      baseband_gateway& bb_gateway = session->get_baseband_gateway(stream_id);

      // Select receiver.
      baseband_gateway_receiver& receiver = bb_gateway.get_receiver();

      // Receive.
      baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());
      TESTASSERT_EQ(md.ts, start_time + block_id * config.block_size);

      // Validate data for each channel.
      for (unsigned channel_id = 0; channel_id != config.nof_channels; ++channel_id) {
        span<const cf_t> buffer = rx_buffer[channel_id];
        for (const cf_t& sample : buffer) {
          cf_t  expected_sample = {dist(rx_rgen), dist(rx_rgen)};
          float error           = std::abs(sample - expected_sample);
          TESTASSERT(error < ASSERT_MAX_ERROR,
                     "Error ({}) exceeds maximum ({}). Unmatched data {}!={}",
                     error,
                     ASSERT_MAX_ERROR,
                     expected_sample,
                     sample);
        }
      }
    }
  }

  // Stop session.
  session->stop();
}

static std::vector<std::string> get_zmq_ports(unsigned nof_ports)
{
  std::vector<std::string> result;

  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, "inproc://{}#{}", getpid(), i_port);
    result.emplace_back(to_string(buffer));
  }

  return result;
}

int main()
{
  srslog::init();

  // Common test parameters.
  std::string log_level = "warning";

  // Create ZMQ factory.
  std::unique_ptr<radio_factory> factory = create_radio_factory("zmq");
  TESTASSERT(factory != nullptr);

  // Asynchronous task executor.
  task_worker                    async_task_worker("async_thread", 2 * RADIO_MAX_NOF_PORTS);
  std::unique_ptr<task_executor> async_task_executor = make_task_executor(async_task_worker);
  srslog::fetch_basic_logger("POOL").set_level(srslog::str_to_basic_level(log_level));

  {
    test_description test_config;
    test_config.nof_streams    = 1;
    test_config.nof_channels   = 1;
    test_config.sample_rate_hz = 1.92e6;
    test_config.tx_freq_hz     = 3.5e9;
    test_config.tx_gain_db     = 60.0;
    test_config.tx_addresses   = get_zmq_ports(1);
    test_config.rx_freq_hz     = 3.5e9;
    test_config.rx_gain_db     = 60.0;
    test_config.rx_addresses   = get_zmq_ports(1);
    test_config.log_level      = log_level;
    test_config.block_size     = 1024;
    test_config.nof_blocks     = 10;
    test(test_config, *factory, *async_task_executor);
  }

  {
    test_description test_config;
    test_config.nof_streams    = 2;
    test_config.nof_channels   = 2;
    test_config.sample_rate_hz = 3.84e6;
    test_config.tx_freq_hz     = 3.5e9;
    test_config.tx_gain_db     = 60.0;
    test_config.tx_addresses   = get_zmq_ports(4);
    test_config.rx_freq_hz     = 3.5e9;
    test_config.rx_gain_db     = 60.0;
    test_config.rx_addresses   = get_zmq_ports(4);
    test_config.log_level      = log_level;
    test_config.block_size     = 128;
    test_config.nof_blocks     = 10;
    test(test_config, *factory, *async_task_executor);
  }

  async_task_worker.stop();

  return 0;
}
