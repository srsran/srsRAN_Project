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

#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/support/complex_normal_random.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

extern "C" {
#include <trx_driver.h>
}

using namespace srsran;

namespace {
class radio_notification_handler_impl : public radio_notification_handler
{
private:
  int64_t tx_underflow_count = 0;
  int64_t rx_overflow_count  = 0;

public:
  void on_radio_rt_event(const event_description& description) override
  {
    if ((description.source == event_source::TRANSMIT) &&
        ((description.type == event_type::UNDERFLOW) || (description.type == event_type::LATE))) {
      fmt::print("Underflow!\n");
      ++tx_underflow_count;
      return;
    }

    if (description.source == event_source::RECEIVE && description.type == event_type::OVERFLOW) {
      ++rx_overflow_count;
      return;
    }
  }

  int64_t get_tx_underflow_count() const { return tx_underflow_count; }

  int64_t get_rx_overflow_count() const { return rx_overflow_count; }
};

class baseband_gateway_buffer_trx : public baseband_gateway_buffer_reader, public baseband_gateway_buffer_writer
{
private:
  cf_t**   buffer;
  unsigned nof_samples;
  unsigned nof_channels;

public:
  /// Constructs the buffer from a read-only buffer for the transmitter.
  baseband_gateway_buffer_trx(const void** psamples, int nof_samples_, int nof_channels_) :
    buffer((cf_t**)(psamples)), nof_samples(nof_samples_), nof_channels(nof_channels_)
  {
    // Do nothing.
  }

  /// Constructs the buffer from a buffer for the receiver.
  baseband_gateway_buffer_trx(void** psamples, int nof_samples_, int nof_channels_) :
    buffer((cf_t**)(psamples)), nof_samples(nof_samples_), nof_channels(nof_channels_)
  {
    // Do nothing.
  }

  // See interface for documentation
  unsigned get_nof_channels() const override { return nof_channels; }

  // See interface for documentation
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation
  span<cf_t> get_channel_buffer(unsigned channel_idx) override { return span<cf_t>(buffer[channel_idx], nof_samples); }

  // See interface for documentation
  span<const cf_t> get_channel_buffer(unsigned channel_idx) const override
  {
    return span<cf_t>(buffer[channel_idx], nof_samples);
  }
};

class baseband_gateway_buffer_zeros : public baseband_gateway_buffer_reader
{
private:
  unsigned nof_channels;
  unsigned nof_samples;

  static std::mutex        data_mutex;
  static std::vector<cf_t> data;

public:
  baseband_gateway_buffer_zeros(unsigned nof_channels_, unsigned nof_samples_) :
    nof_channels(nof_channels_), nof_samples(nof_samples_)
  {
    std::unique_lock<std::mutex> lock(data_mutex);
    if (data.size() < nof_samples) {
      data.resize(nof_samples, cf_t());
    }
  }

  // See interface for documentation
  unsigned get_nof_channels() const override { return nof_channels; }

  // See interface for documentation
  unsigned get_nof_samples() const override { return nof_samples; }

  // See interface for documentation
  span<const cf_t> get_channel_buffer(unsigned) const override { return span<const cf_t>(data).first(nof_samples); }
};

std::mutex        baseband_gateway_buffer_zeros::data_mutex;
std::vector<cf_t> baseband_gateway_buffer_zeros::data(61440, cf_t());

} // namespace

// Groups parameters that describe a trx_srsran session.
struct trx_srsran_session_context {
  // Parameters.
  radio_configuration::over_the_wire_format    otw_format;
  int                                          rf_port_count;
  std::array<int, TRX_MAX_RF_PORT>             tx_port_channel_count;
  std::array<int, TRX_MAX_RF_PORT>             rx_port_channel_count;
  std::array<std::string, RADIO_MAX_NOF_PORTS> tx_port_args;
  std::array<std::string, RADIO_MAX_NOF_PORTS> rx_port_args;
  std::string                                  factory_str;
  std::string                                  log_level;
  // Radio factory.
  std::unique_ptr<radio_factory> factory;
  // Radio session.
  std::unique_ptr<radio_session> session;
  // Task worker.
  std::unique_ptr<task_worker> async_task_worker;
  // Asynchronous task executor.
  std::unique_ptr<task_executor> async_task_executor;
  // Notification handler.
  radio_notification_handler_impl notification_handler;
  // States.
  std::array<double, TRX_MAX_RF_PORT> tx_port_channel_gain;
  std::array<double, TRX_MAX_RF_PORT> rx_port_channel_gain;
  unsigned                            tx_samples_per_packet;
  // Transmit noise spectral density in dB/Hz if present. Otherwise, noise generator is disabled.
  optional<float> noise_spd = nullopt;
  // Random generator.
  std::mt19937 rgen;
  // Random distribution for AWGN.
  complex_normal_distribution<> dist;
};

// Keeps persistent trx_srsran sessions.
static std::vector<std::unique_ptr<trx_srsran_session_context>> trx_srsran_sessions;

#if TRX_API_VERSION > 12
static void trx_srsran_write2(TRXState*         s1,
                              trx_timestamp_t   timestamp,
                              const void**      psamples,
                              int               count,
                              int               tx_port_index,
                              TRXWriteMetadata* md)
{
  // Empirically this avoids lates/underflows.
  thread_local bool affinity = false;
  if (!affinity) {
    pthread_t tself = pthread_self();

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (pthread_setaffinity_np(tself, sizeof(cpu_set_t), &cpuset) != 0) {
      perror("pthread_setaffinity_np trx_srsran_read2");
    }

    sched_param param{};
    param.sched_priority = 99;
    if (pthread_setschedparam(tself, SCHED_FIFO, &param) != 0) {
      fprintf(stderr,
              "Warning: Scheduling priority of thread \"%s\" not changed. Cause: Not enough privileges.\n",
              this_thread_name());
    }

    affinity = true;
  }

  srsran_assert(s1, "Invalid state.");
  srsran_assert(md, "Invalid metadata.");

  // Extract context and transmitter interface.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);
  srsran_assert(context.session, "Invalid session.");
  baseband_gateway_transmitter& transmitter = context.session->get_baseband_gateway(tx_port_index).get_transmitter();

  bool padding_request  = md->flags & TRX_WRITE_MD_FLAG_PADDING;
  bool end_of_burst     = md->flags & TRX_WRITE_MD_FLAG_END_OF_BURST;
  bool harq_ack_present = md->flags & TRX_WRITE_MD_FLAG_HARQ_ACK_PRESENT;
  bool ta_present       = md->flags & TRX_WRITE_MD_FLAG_TA_PRESENT;
  bool request_ts       = md->flags & TRX_WRITE_MD_FLAG_CUR_TIMESTAMP_REQ;

  // Ignore transmission if it is padding or an end of burst with zero samples to transmit.
  if (end_of_burst && (count == 0)) {
    return;
  }

  if (harq_ack_present) {
    fmt::print("HARQ ACK is not implemented.\n");
    return;
  }

  if (ta_present) {
    fmt::print("TA is not implemented.\n");
    return;
  }

  // Prepare metadata.
  baseband_gateway_transmitter_metadata metadata;
  metadata.is_empty = false;
  metadata.ts       = static_cast<baseband_gateway_timestamp>(timestamp);

  // Write metadata.
  md->cur_timestamp_set = 0;
  if (request_ts) {
    md->cur_timestamp = timestamp + count;
  }

  // Send zeros if padding is signaled.
  if (padding_request) {
    baseband_gateway_buffer_zeros buffer(context.tx_port_channel_count[tx_port_index], count);

    // Transmit zeros.
    transmitter.transmit(buffer, metadata);

    return;
  }

  // Prepare buffer.
  baseband_gateway_buffer_trx buffer(psamples, count, context.tx_port_channel_count[tx_port_index]);

  // Add noise if enabled.
  if (context.noise_spd.has_value()) {
    for (unsigned i_chan = 0, nof_channels = context.tx_port_channel_count[tx_port_index]; i_chan != nof_channels;
         ++i_chan) {
      span<cf_t> chan_buffer = buffer.get_channel_buffer(i_chan);
      std::transform(chan_buffer.begin(), chan_buffer.end(), chan_buffer.begin(), [&context](cf_t sample) {
        return sample + context.dist(context.rgen);
      });
    }
  }

  // Transmit baseband.
  transmitter.transmit(buffer, metadata);
}

static int trx_srsran_read2(TRXState*        s1,
                            trx_timestamp_t* ptimestamp,
                            void**           psamples,
                            int              count,
                            int              rf_port_index,
                            TRXReadMetadata* md)
{
  // Empirically this avoids lates/underflows.
  thread_local bool affinity = false;
  if (!affinity) {
    pthread_t tself = pthread_self();

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (pthread_setaffinity_np(tself, sizeof(cpu_set_t), &cpuset) != 0) {
      perror("pthread_setaffinity_np trx_srsran_read2");
    }

    sched_param param{};
    param.sched_priority = 90;
    if (pthread_setschedparam(tself, SCHED_FIFO, &param) != 0) {
      fprintf(stderr,
              "Warning: Scheduling priority of thread \"%s\" not changed. Cause: Not enough privileges.\n",
              this_thread_name());
    }

    affinity = true;
  }
  std::this_thread::sleep_for(std::chrono::microseconds(1));

  srsran_assert(s1, "Invalid state.");

  // Extract context and receiver interface.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);
  srsran_assert(context.session, "Invalid session.");
  baseband_gateway_receiver& receiver = context.session->get_baseband_gateway(rf_port_index).get_receiver();

  // if (md != nullptr && md->flags) {
  //   fmt::print("Read2 flags {} not implemented.", md->flags);
  // }

  // Prepare buffer.
  baseband_gateway_buffer_trx buffer(psamples, count, context.rx_port_channel_count[rf_port_index]);

  // Receive baseband.
  baseband_gateway_receiver::metadata metadata = receiver.receive(buffer);

  // Fill reception metadata.
  if (ptimestamp != nullptr) {
    *ptimestamp = static_cast<trx_timestamp_t>(metadata.ts);
  }

  return count;
}
#endif /* TRX_API_VERSION > 12 */

static void trx_srsran_set_tx_gain(TRXState* s1, double gain, int chain)
{
  srsran_assert(s1, "Invalid state.");

  // Extract context and receiver interface.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);
  srsran_assert(context.session, "Invalid session.");
  radio_management_plane& control = context.session->get_management_plane();

  control.set_tx_gain(chain, gain);

  context.tx_port_channel_gain[chain] = gain;
}

static void trx_srsran_set_rx_gain(TRXState* s1, double gain, int chain)
{
  srsran_assert(s1, "Invalid state.");

  // Extract context and receiver interface.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);
  srsran_assert(context.session, "Invalid session.");
  radio_management_plane& control = context.session->get_management_plane();

  control.set_rx_gain(chain, gain);

  context.rx_port_channel_gain[chain] = gain;
}

static int trx_srsran_start(TRXState* s1, const TRXDriverParams* p)
{
  srsran_assert(s1, "Invalid state.");
  srsran_assert(p, "Invalid parameters.");

  // Extract context.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);

  // Create factory.
  context.factory = create_radio_factory(context.factory_str);
  if (!context.factory) {
    fmt::print("Failed to create Radio factory {}.", context.factory_str);
    return -1;
  }

  // Get number of RF streams.
  context.rf_port_count = p->rf_port_count;
  srsran_assert(context.rf_port_count > 0, "At least one port is required.");

  // Deduce sampling rate.
  srsran_assert((p->sample_rate[0].num > 0) && (p->sample_rate[0].den > 0),
                "Invalid sampling rate num={}, den={}.",
                p->sample_rate[0].num,
                p->sample_rate[0].den);
  double sample_rate_Hz = static_cast<double>(p->sample_rate->num) / static_cast<double>(p->sample_rate[0].den);

  // Prepare noise generator.
  if (context.noise_spd.has_value()) {
    // Convert to standard deviation and initialize noise generator.
    float noise_std = std::sqrt(convert_dB_to_power(context.noise_spd.value()) * sample_rate_Hz);
    context.dist    = complex_normal_distribution<>(0.0, noise_std);
  }

  // Check that all sampling rates for all channels are the same.
  span<const TRXFraction> sampling_rates_frac =
      span<const TRXFraction>(p->sample_rate, context.rf_port_count).last(context.rf_port_count - 1);
  std::all_of(sampling_rates_frac.begin(), sampling_rates_frac.end(), [&](const TRXFraction& x) {
    return x.num == p->sample_rate[0].num && x.den == p->sample_rate[0].den;
  });

  // Prepare configuration.
  radio_configuration::radio configuration = {};
  configuration.sampling_rate_hz           = sample_rate_Hz;
  configuration.log_level                  = context.log_level;
  configuration.otw_format                 = context.otw_format;

  // Map TRX ports into streams.
  for (int stream = 0, tx_port_count = 0, rx_port_count = 0; stream < context.rf_port_count; stream++) {
    context.tx_port_channel_count[stream] = p->tx_port_channel_count[stream];
    context.rx_port_channel_count[stream] = p->rx_port_channel_count[stream];

    // Prepare Tx stream configuration.
    configuration.tx_streams.emplace_back();
    radio_configuration::stream& tx_stream_config = configuration.tx_streams.back();

    // Iterate all transmit channels of the stream.
    for (int channel = 0; channel != context.tx_port_channel_count[stream]; ++channel) {
      // Prepare transmit channel configuration.
      tx_stream_config.channels.emplace_back();
      radio_configuration::channel& tx_channel_config = tx_stream_config.channels.back();

      tx_channel_config.freq.center_frequency_hz  = static_cast<double>(p->tx_freq[tx_port_count]);
      tx_channel_config.gain_dB                   = static_cast<double>(p->tx_gain[tx_port_count]);
      tx_channel_config.args                      = context.tx_port_args[tx_port_count];
      context.tx_port_channel_gain[rx_port_count] = p->tx_gain[tx_port_count];
      ++tx_port_count;
    }

    // Prepare Rx stream configuration.
    configuration.rx_streams.emplace_back();
    radio_configuration::stream& rx_stream_config = configuration.rx_streams.back();

    // Iterate all receive channels of the stream.
    for (int channel = 0; channel != p->rx_port_channel_count[stream]; ++channel) {
      // Prepare transmit channel configuration.
      rx_stream_config.channels.emplace_back();
      radio_configuration::channel& rx_channel_config = rx_stream_config.channels.back();

      rx_channel_config.freq.center_frequency_hz  = static_cast<double>(p->rx_freq[rx_port_count]);
      rx_channel_config.gain_dB                   = static_cast<double>(p->rx_gain[rx_port_count]);
      rx_channel_config.args                      = context.rx_port_args[rx_port_count];
      context.rx_port_channel_gain[rx_port_count] = p->rx_gain[rx_port_count];
      ++rx_port_count;
    }
  }

  // Create task worker.
  context.async_task_worker   = std::make_unique<task_worker>("async_thread", p->rf_port_count + 10);
  context.async_task_executor = make_task_executor_ptr(*context.async_task_worker);

  // Create radio session.
  context.session = context.factory->create(configuration, *context.async_task_executor, context.notification_handler);
  report_fatal_error_if_not(context.session, "Invalid session.");

  context.session->start(0);

  // Half millisecond packets.
  context.tx_samples_per_packet = p->sample_rate[0].num / 500;

  return 0;
}

static void trx_srsran_end(TRXState* s1)
{
  srsran_assert(s1, "Invalid state.");

  // Extract context.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);

  // Select session and stop.
  srsran_assert(context.session, "Invalid session.");
  context.session->stop();
}

static int
trx_srsran_get_sample_rate(TRXState* s1, TRXFraction* psample_rate, int* psample_rate_num, int sample_rate_min)
{
  if (psample_rate) {
    psample_rate->num = sample_rate_min;
    psample_rate->den = 1;
  }

  if (psample_rate_num) {
    psample_rate_num[0] = 1;
    psample_rate_num[1] = 0;
    psample_rate_num[2] = 0;
  }

  return 0;
}

static int trx_srsran_get_tx_samples_per_packet(TRXState* s1)
{
  srsran_assert(s1, "Invalid state.");
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);

  return context.tx_samples_per_packet;
}

static int trx_srsran_get_stats(TRXState* s1, TRXStatistics* m)
{
  srsran_assert(s1, "Invalid state.");

  // Extract context.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);

  m->tx_underflow_count = context.notification_handler.get_tx_underflow_count();
  m->rx_overflow_count  = context.notification_handler.get_rx_overflow_count();
  return 0;
}

static void trx_srsran_get_tx_gain(TRXState* s1, double* gain, int channel_num)
{
  srsran_assert(s1, "Invalid state.");

  // Extract context.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);

  if (gain) {
    *gain = context.tx_port_channel_gain[channel_num];
  }
}

static void trx_srsran_get_rx_gain(TRXState* s1, double* gain, int channel_num)
{
  srsran_assert(s1, "Invalid state.");

  // Extract context.
  trx_srsran_session_context& context = *static_cast<trx_srsran_session_context*>(s1->opaque);

  if (gain) {
    *gain = context.rx_port_channel_gain[channel_num];
  }
}

static void trx_srsran_dump_info(TRXState* s1, trx_printf_cb cb, void* opaque)
{
  srsran_assert(s1, "Invalid state.");

  cb(opaque, "Using trx_srsran module\n");
}

int trx_driver_init(TRXState* s1)
{
  srslog::init();
  srsran_assert(s1, "Invalid state.");

  srsran_assert(s1->trx_api_version == TRX_API_VERSION,
                "ABI compatibility mismatch between LTEENB and TRX UHD driver (LTEENB ABI version={}, TRX UHD driver "
                "ABI version={}).",
                s1->trx_api_version,
                TRX_API_VERSION);

  // Create new session context.
  trx_srsran_sessions.emplace_back(std::make_unique<trx_srsran_session_context>());

  // Select context.
  trx_srsran_session_context& context = *trx_srsran_sessions.back();

  // Parse radio arguments.
  char* radio_factory_char = trx_get_param_string(s1, "factory");
  if (radio_factory_char != nullptr) {
    context.factory_str = std::string(radio_factory_char);
    free(radio_factory_char);
  } else {
    context.factory_str = "zmq";
  }

  // Parse OTW format.
  char*       otw_format_char = trx_get_param_string(s1, "otw_format");
  std::string otw_format_str  = "default";
  if (otw_format_char) {
    otw_format_str = std::string(otw_format_char);
    free(otw_format_char);
  }
  if (otw_format_str == "default") {
    context.otw_format = radio_configuration::over_the_wire_format::DEFAULT;
  } else if (otw_format_str == "sc16") {
    context.otw_format = radio_configuration::over_the_wire_format::SC16;
  } else {
    srsran_terminate("Invalid OTW format {}. Supported formats are: default, sc16.", otw_format_str);
  }

  // Parse transmit ports arguments.
  for (unsigned port = 0; port < RADIO_MAX_NOF_PORTS; ++port) {
    // Create port field name.
    fmt::memory_buffer fmt_format_buf;
    fmt::format_to(fmt_format_buf, "tx_port{}", port);

    // Get parameter from configuration.
    char* tx_port_char = trx_get_param_string(s1, to_string(fmt_format_buf).c_str());

    // Set parameter if it is available.
    if (tx_port_char != nullptr) {
      context.tx_port_args[port] = std::string(tx_port_char);
      free(tx_port_char);
    }
  }

  // Parse receive ports arguments.
  for (unsigned port = 0; port < RADIO_MAX_NOF_PORTS; ++port) {
    // Create port field name.
    fmt::memory_buffer fmt_format_buf;
    fmt::format_to(fmt_format_buf, "rx_port{}", port);

    // Get parameter from configuration.
    char* rx_port_char = trx_get_param_string(s1, to_string(fmt_format_buf).c_str());

    // Set parameter if it is available.
    if (rx_port_char != nullptr) {
      context.rx_port_args[port] = std::string(rx_port_char);
      free(rx_port_char);
    }
  }

  // Parse logging level.
  char* log_level_char = trx_get_param_string(s1, "log_level");
  if (log_level_char != nullptr) {
    context.log_level = std::string(log_level_char);
    free(log_level_char);
  } else {
    context.log_level = "info";
  }

  // Parse noise spectral density.
  double noise_spd_double;
  if (trx_get_param_double(s1, &noise_spd_double, "noise_spd") == 0) {
    context.noise_spd.emplace(static_cast<float>(noise_spd_double));
  }

  s1->opaque         = &context;
  s1->trx_end_func   = trx_srsran_end;
  s1->trx_start_func = trx_srsran_start;
#if TRX_API_VERSION > 12
  s1->trx_write_func2 = trx_srsran_write2;
  s1->trx_read_func2  = trx_srsran_read2;
#endif /* TRX_API_VERSION > 12 */
  s1->trx_set_tx_gain_func               = trx_srsran_set_tx_gain;
  s1->trx_set_rx_gain_func               = trx_srsran_set_rx_gain;
  s1->trx_get_sample_rate_func           = trx_srsran_get_sample_rate;
  s1->trx_get_tx_samples_per_packet_func = trx_srsran_get_tx_samples_per_packet;
  s1->trx_get_stats                      = trx_srsran_get_stats;
  s1->trx_dump_info                      = trx_srsran_dump_info;
  s1->trx_get_tx_gain_func               = trx_srsran_get_tx_gain;
  s1->trx_get_rx_gain_func               = trx_srsran_get_rx_gain;
  s1->trx_stop_func                      = nullptr;

  return 0;
}
