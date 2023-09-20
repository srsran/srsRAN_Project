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
#include "srsran/support/complex_normal_random.h"
#include "srsran/support/executors/task_worker.h"
#include <gtest/gtest.h>
#include <random>
#include <unistd.h>

using namespace srsran;

// Parameters:
// - Number of streams;
// - Number of channels;
// - Transmit block size;
// - Receive block size;
// - Number of samples to transmit/receive; and
// - Throttle TX.
using radio_zmq_e2e_test_parameters = std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned, bool>;

/// Indicates the test logging level.
static const std::string log_level = "warning";

class RadioZmqE2EFixture : public ::testing::TestWithParam<radio_zmq_e2e_test_parameters>
{
protected:
  static constexpr float ASSERT_MAX_ERROR = 1e-6;

  class radio_notifier_spy : public radio_notification_handler
  {
  public:
    void on_radio_rt_event(const event_description& description) override {}
  };

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
  /// Provides the number of samples per transmission.
  unsigned tx_block_size;
  /// Provides the number of samples per reception.
  unsigned rx_block_size;
  /// Total number of samples to process.
  unsigned nof_samples;
  /// Set to true for throttling the transmitter.
  bool throttle_tx;

  static std::unique_ptr<radio_factory> factory;
  static task_worker                    async_task_worker;
  std::vector<std::mt19937>             tx_rgen;
  std::vector<std::mt19937>             rx_rgen;
  complex_normal_distribution<cf_t>     tx_dist;
  complex_normal_distribution<cf_t>     rx_dist;

  static void SetUpTestSuite()
  {
    if (factory) {
      return;
    }

    // Create ZMQ factory.
    factory = create_radio_factory("zmq");
    ASSERT_NE(factory, nullptr);

    srslog::init();
    srslog::fetch_basic_logger("POOL").set_level(srslog::str_to_basic_level(log_level));
  }

  static void TearDownTestSuite() { async_task_worker.stop(); }

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

  void SetUp() override
  {
    // Actual parameters.
    nof_streams   = std::get<0>(GetParam());
    nof_channels  = std::get<1>(GetParam());
    tx_block_size = std::get<2>(GetParam());
    rx_block_size = std::get<3>(GetParam());
    nof_samples   = std::get<4>(GetParam());
    throttle_tx   = std::get<5>(GetParam());

    // Derived parameters.
    unsigned nof_ports = nof_streams * nof_channels;
    tx_addresses       = get_zmq_ports(nof_ports);
    rx_addresses       = get_zmq_ports(nof_ports);

    // Fix parameters.
    sample_rate_hz = 3.84e6;
    tx_freq_hz     = 3.5e9;
    tx_gain_db     = 60.0;
    rx_freq_hz     = 3.5e9;
    rx_gain_db     = 60.0;

    // Setup random generators.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      unsigned seed = 0x1234 + i_port;
      tx_rgen.emplace_back(std::mt19937(seed));
      rx_rgen.emplace_back(std::mt19937(seed));
    }
  }
};

class radio_notifier_spy : public radio_notification_handler
{
public:
  void on_radio_rt_event(const event_description& description) override {}
};

std::unique_ptr<radio_factory> RadioZmqE2EFixture::factory = nullptr;
task_worker                    RadioZmqE2EFixture::async_task_worker("async_thread", 2 * RADIO_MAX_NOF_PORTS);

TEST_P(RadioZmqE2EFixture, RadioZmqE2EFlow)
{
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor = make_task_executor(async_task_worker);

  // Prepare radio configuration.
  radio_configuration::radio radio_config;
  for (unsigned stream_id = 0, port_index = 0; stream_id != nof_streams; ++stream_id) {
    radio_configuration::stream stream_config;
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id, ++port_index) {
      radio_configuration::channel channel_config;
      channel_config.freq.center_frequency_hz = tx_freq_hz;
      channel_config.gain_dB                  = tx_gain_db;
      channel_config.args                     = tx_addresses[port_index];
      stream_config.channels.push_back(channel_config);
    }
    radio_config.tx_streams.push_back(stream_config);
  }

  for (unsigned stream_id = 0, port_index = 0; stream_id != nof_streams; ++stream_id) {
    radio_configuration::stream stream_config;
    for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id, ++port_index) {
      radio_configuration::channel channel_config;
      channel_config.freq.center_frequency_hz = rx_freq_hz;
      channel_config.gain_dB                  = rx_gain_db;
      channel_config.args                     = rx_addresses[port_index];
      stream_config.channels.push_back(channel_config);
    }
    radio_config.rx_streams.push_back(stream_config);
  }
  radio_config.log_level        = log_level;
  radio_config.sampling_rate_hz = sample_rate_hz;

  // Notifier.
  radio_notifier_spy radio_notifier;

  // Create radio session.
  std::unique_ptr<radio_session> session = factory->create(radio_config, *async_task_executor, radio_notifier);
  ASSERT_NE(session, nullptr);

  // Calculate starting time.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = session->read_current_time();
  baseband_gateway_timestamp start_time = current_time + static_cast<uint64_t>(delay_s * radio_config.sampling_rate_hz);

  // Start processing.
  session->start(start_time);

  std::atomic<unsigned> tx_sample_count = {0};
  std::atomic<unsigned> rx_sample_count = {0};

  std::thread tx_thread([this, &session, &start_time, &tx_sample_count, &rx_sample_count] {
    // Prepare transmit buffer
    baseband_gateway_buffer_dynamic tx_buffer(nof_channels, tx_block_size);

    while (tx_sample_count != nof_samples) {
      // If the transmitter throttling is enabled, make sure the transmitter does not advance the receiver.
      while (throttle_tx && (tx_sample_count > (rx_sample_count + 2 * rx_block_size))) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
      }

      unsigned block_size = std::min(tx_block_size, nof_samples - tx_sample_count);
      tx_buffer.resize(block_size);

      // Transmit for each stream the same buffer.
      for (unsigned stream_id = 0, port_id = 0; stream_id != nof_streams; ++stream_id) {
        // Get baseband gateway.
        baseband_gateway& bb_gateway = session->get_baseband_gateway(stream_id);

        // Select transmitter.
        baseband_gateway_transmitter& transmitter = bb_gateway.get_transmitter();

        // Generate transmit random data for each channel.
        for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id, ++port_id) {
          span<cf_t> buffer = tx_buffer[channel_id];
          for (cf_t& sample : buffer) {
            sample = tx_dist(tx_rgen[port_id]);
          }
        }

        // Transmit stream buffer.
        baseband_gateway_transmitter::metadata tx_md;
        tx_md.ts = start_time + tx_sample_count;
        transmitter.transmit(tx_buffer.get_reader(), tx_md);
      }

      tx_sample_count += block_size;
    }
  });

  // Prepare receive buffer.
  baseband_gateway_buffer_dynamic rx_buffer(nof_channels, rx_block_size);

  while (rx_sample_count != nof_samples) {
    // Calculate block size.
    unsigned block_size = std::min(rx_block_size, nof_samples - rx_sample_count);
    rx_buffer.resize(block_size);

    // Make sure the receiver does not advance the transmitter.
    while (tx_sample_count < rx_sample_count + block_size) {
      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    // Receive for each stream the same buffer.
    for (unsigned stream_id = 0, port_id = 0; stream_id != nof_streams; ++stream_id) {
      // Get baseband gateway.
      baseband_gateway& bb_gateway = session->get_baseband_gateway(stream_id);

      // Select receiver.
      baseband_gateway_receiver& receiver = bb_gateway.get_receiver();

      // Receive.
      baseband_gateway_receiver::metadata md = receiver.receive(rx_buffer.get_writer());
      ASSERT_EQ(md.ts, start_time + rx_sample_count);

      // Validate data for each channel.
      for (unsigned channel_id = 0; channel_id != nof_channels; ++channel_id, ++port_id) {
        span<const cf_t> buffer = rx_buffer[channel_id];
        for (const cf_t& sample : buffer) {
          cf_t expected_sample = rx_dist(rx_rgen[port_id]);
          EXPECT_NEAR(expected_sample.real(), sample.real(), ASSERT_MAX_ERROR);
          EXPECT_NEAR(expected_sample.imag(), sample.imag(), ASSERT_MAX_ERROR);
        }
      }
    }

    rx_sample_count += block_size;
  }

  // Finish Tx thread.
  tx_thread.join();

  // Stop session.
  session->stop();
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(RadioZmqE2ETest,
                         RadioZmqE2EFixture,
                         ::testing::Combine(::testing::Values(1, 2),
                                            ::testing::Values(1, 2),
                                            ::testing::Values(39, 123),
                                            ::testing::Values(39, 123),
                                            ::testing::Values(61440),
                                            ::testing::Values(false, true)));
