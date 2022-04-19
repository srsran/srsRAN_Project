
#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/math_utils.h"
#include <csignal>
#include <random>

using namespace srsgnb;

class radio_notifier_spy : public radio_notification_handler
{
private:
  srslog::basic_logger& logger;
  unsigned              count_tx_overflow  = 0;
  unsigned              count_tx_underflow = 0;
  unsigned              count_tx_late      = 0;
  unsigned              count_tx_other     = 0;
  unsigned              count_rx_overflow  = 0;
  unsigned              count_rx_underflow = 0;
  unsigned              count_rx_late      = 0;
  unsigned              count_rx_other     = 0;

public:
  radio_notifier_spy(std::string log_level_) : logger(srslog::fetch_basic_logger("Radio notification"))
  {
    srslog::init();
    logger.set_level(srslog::str_to_basic_level(log_level_));
  }

  void on_radio_rt_event(const event_description& description) override
  {
    logger.info("stream_id={} channel_id={} source={} type={}",
                description.stream_id == UNKNOWN_ID ? "na" : std::to_string(description.stream_id),
                description.channel_id == UNKNOWN_ID ? "na" : std::to_string(description.channel_id),
                description.source.to_string(),
                description.type.to_string());
    switch (description.type) {
      case event_type::UNDEFINED:
        // Ignore.
        break;
      case event_type::LATE:
        if (description.source == event_source::TRANSMIT) {
          count_tx_late++;
        } else {
          count_rx_late++;
        }
        break;
      case event_type::UNDERFLOW:
        if (description.source == event_source::TRANSMIT) {
          count_tx_underflow++;
        } else {
          count_rx_underflow++;
        }
        break;
      case event_type::OVERFLOW:
        if (description.source == event_source::TRANSMIT) {
          count_tx_overflow++;
        } else {
          count_rx_overflow++;
        }
        break;
      case event_type::OTHER:
        if (description.source == event_source::TRANSMIT) {
          count_tx_other++;
        } else {
          count_rx_other++;
        }
        break;
    }
  }

  void print()
  {
    printf("[TX] Overflow: %d; Late: %d Underflow: %d Other: %d\n",
           count_tx_overflow,
           count_tx_late,
           count_tx_underflow,
           count_tx_other);
    printf("[RX] Overflow: %d; Late: %d Underflow: %d Other: %d\n",
           count_rx_overflow,
           count_rx_late,
           count_rx_underflow,
           count_rx_other);
  }
};

std::atomic<bool> stop = {false};

void signal_handler(int sig)
{
  stop = true;
}

int main(int argc, char** argv)
{
  // Program parameters.
  std::string                               driver_name             = "uhd";
  std::string                               device_arguments        = "type=b200";
  std::string                               log_level               = "info";
  double                                    sampling_rate_hz        = 30.72e6;
  unsigned                                  nof_tx_streams          = 1;
  unsigned                                  nof_rx_streams          = 1;
  unsigned                                  nof_channels_per_stream = 1;
  unsigned                                  block_size              = sampling_rate_hz / 15e3;
  double                                    tx_freq                 = 3.5e9;
  double                                    tx_gain                 = 60.0;
  double                                    rx_freq                 = 3.5e9;
  double                                    rx_gain                 = 60.0;
  double                                    duration_s              = 10.0;
  double                                    tx_rx_delay_s           = 0.001;
  radio_configuration::over_the_wire_format otw_format              = radio_configuration::over_the_wire_format::SC16;

  if (argc > 1 && argv[1] != nullptr) {
    if (std::strcmp(argv[1], "b200_50MHz") == 0) {
      device_arguments = "type=b200";
      sampling_rate_hz = 61.44e6;
      otw_format       = radio_configuration::over_the_wire_format::SC12;
      block_size       = sampling_rate_hz / 15e3;
    } else if (std::strcmp(argv[1], "x300_20MHz") == 0) {
      device_arguments = "type=x300,master_clock_rate=184.32e6,send_frame_size=8000,recv_frame_size=8000";
      sampling_rate_hz = 23.04e6;
      otw_format       = radio_configuration::over_the_wire_format::SC16;
      block_size       = sampling_rate_hz / 15e3;
      tx_gain          = 10.0;
      rx_gain          = 10.0;
    } else if (std::strcmp(argv[1], "x300_50MHz") == 0) {
      device_arguments = "type=x300,master_clock_rate=184.32e6,send_frame_size=8000,recv_frame_size=8000";
      sampling_rate_hz = 92.16e6;
      otw_format       = radio_configuration::over_the_wire_format::SC16;
      block_size       = sampling_rate_hz / 15e3;
      tx_gain          = 10.0;
      rx_gain          = 10.0;
    } else if (std::strcmp(argv[1], "x300_100MHz") == 0) {
      device_arguments = "type=x300,master_clock_rate=184.32e6,send_frame_size=8000,recv_frame_size=8000";
      sampling_rate_hz = 184.32e6;
      otw_format       = radio_configuration::over_the_wire_format::SC16;
      block_size       = sampling_rate_hz / 15e3;
      tx_gain          = 10.0;
      rx_gain          = 10.0;
    } else if (std::strcmp(argv[1], "x300_20MHz_2x2") == 0) {
      device_arguments        = "type=x300,master_clock_rate=184.32e6,send_frame_size=8000,recv_frame_size=8000";
      sampling_rate_hz        = 23.04e6;
      otw_format              = radio_configuration::over_the_wire_format::SC16;
      block_size              = sampling_rate_hz / 15e3;
      tx_gain                 = 10.0;
      rx_gain                 = 10.0;
      nof_channels_per_stream = 2;
    } else if (std::strcmp(argv[1], "x300_50MHz_2x2") == 0) {
      device_arguments        = "type=x300,master_clock_rate=184.32e6,send_frame_size=8000,recv_frame_size=8000";
      sampling_rate_hz        = 92.16e6;
      otw_format              = radio_configuration::over_the_wire_format::SC16;
      block_size              = sampling_rate_hz / 15e3;
      tx_gain                 = 10.0;
      rx_gain                 = 10.0;
      nof_channels_per_stream = 1;
      nof_tx_streams          = 2;
      nof_rx_streams          = 2;
    } else if (std::strcmp(argv[1], "x300_100MHz_2x2") == 0) {
      device_arguments        = "type=x300,master_clock_rate=184.32e6,send_frame_size=8000,recv_frame_size=8000";
      sampling_rate_hz        = 184.32e6;
      otw_format              = radio_configuration::over_the_wire_format::SC16;
      block_size              = sampling_rate_hz / 15e3;
      tx_gain                 = 10.0;
      rx_gain                 = 10.0;
      nof_channels_per_stream = 1;
      nof_tx_streams          = 2;
      nof_rx_streams          = 2;
    } else {
      printf("Invalid arguments.\n");
      return -1;
    }
  }

  // Random data generator.
  std::mt19937                          rgen(0);
  std::uniform_real_distribution<float> dist(-1.0, +1.0);

  unsigned tx_rx_delay_samples = static_cast<unsigned>(sampling_rate_hz * tx_rx_delay_s);

  // Asynchronous task executor.
  task_worker                    async_task_worker("async_thread", RADIO_MAX_NOF_PORTS);
  std::unique_ptr<task_executor> async_task_executor = make_task_executor(async_task_worker);

  // Create radio factory.
  std::unique_ptr<radio_factory> factory = create_radio_factory(driver_name);
  srsran_always_assert(factory, "Driver %s is not available.", driver_name.c_str());

  // Create radio configuration.
  radio_configuration::radio config = {};
  config.sampling_rate_hz           = sampling_rate_hz;
  config.otw_format                 = otw_format;
  config.args                       = device_arguments;
  config.log_level                  = "debug";
  radio_configuration::stream rx_stream_config;

  // Create Tx stream and channels.
  std::vector<radio_baseband_buffer_dynamic> tx_baseband_buffers;
  for (unsigned stream_idx = 0; stream_idx != nof_tx_streams; ++stream_idx) {
    // Create transmit baseband buffer for the stream.
    tx_baseband_buffers.emplace_back(nof_channels_per_stream, block_size);

    // For each channel in the stream...
    radio_configuration::stream stream_config;
    for (unsigned channel_idx = 0; channel_idx != nof_channels_per_stream; ++channel_idx) {
      // Create channel configuration and append.
      radio_configuration::channel ch_config;
      ch_config.freq.center_frequency_hz = tx_freq;
      ch_config.gain_dB                  = tx_gain;
      stream_config.channels.emplace_back(ch_config);

      // Generate random data in buffer.
      span<radio_sample_type> data = tx_baseband_buffers.back().get_channel_buffer(channel_idx);
      for (cf_t& iq : data) {
        iq = {dist(rgen), dist(rgen)};
      }
    }
    config.tx_streams.emplace_back(stream_config);
  }

  // Create Rx stream and channels.
  std::vector<radio_baseband_buffer_dynamic> rx_baseband_buffers;
  for (unsigned stream_idx = 0; stream_idx != nof_rx_streams; ++stream_idx) {
    // Create receive baseband buffer for the stream.
    rx_baseband_buffers.emplace_back(nof_channels_per_stream, block_size);

    // For each channel in the stream...
    radio_configuration::stream stream_config;
    for (unsigned channel_idx = 0; channel_idx != nof_channels_per_stream; ++channel_idx) {
      radio_configuration::channel ch_config;
      ch_config.freq.center_frequency_hz = rx_freq;
      ch_config.gain_dB                  = rx_gain;

      stream_config.channels.emplace_back(ch_config);
    }
    config.rx_streams.emplace_back(stream_config);
  }

  // Create notification handler
  radio_notifier_spy notification_handler(log_level);

  // Create radio.
  std::unique_ptr<radio_session> radio = factory->create(config, *async_task_executor, notification_handler);
  srsran_always_assert(radio, "Failed to create radio.");

  // Get transmitter data plane
  radio_data_plane_transmitter& transmitter = radio->get_data_plane().get_transmitter();

  // Get receiver data plane
  radio_data_plane_receiver& receiver = radio->get_data_plane().get_receiver();

  // Set signal handler.
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);

  // Calculate duration in samples.
  uint64_t total_nof_samples = static_cast<uint64_t>(duration_s * sampling_rate_hz);
  uint64_t sample_count      = 0;

  // Receive and transmit per block basis.
  while (!stop && sample_count < total_nof_samples) {
    // For each stream...
    for (unsigned stream_id = 0; stream_id != nof_rx_streams; ++stream_id) {
      // Receive baseband.
      static_vector<radio_data_plane_receiver::metadata, RADIO_MAX_NOF_STREAMS> rx_metadata(nof_rx_streams);
      rx_metadata[stream_id] = receiver.receive(tx_baseband_buffers[stream_id], stream_id);

      // Prepare transmit metadata.
      radio_data_plane_transmitter::metadata tx_metadata = {};
      tx_metadata.ts                                     = rx_metadata.front().ts + tx_rx_delay_samples;

      // Transmit baseband.
      transmitter.transmit(stream_id, tx_metadata, tx_baseband_buffers[stream_id]);
    }

    // Increment sample counter.
    sample_count += block_size;
  }

  // Stop radio operation prior destruction.
  radio->stop();

  // Stop asynchronous thread.
  async_task_worker.stop();

  notification_handler.print();

  return 0;
}