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

#include "radio_uhd_exception_handler.h"
#include "radio_uhd_multi_usrp.h"
#include "radio_uhd_tx_stream_fsm.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/radio/radio_configuration.h"
#include "srsran/radio/radio_notification_handler.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/support/executors/task_executor.h"
#include <mutex>

namespace srsran {

/// Implements a gateway transmitter based on UHD transmit stream.
class radio_uhd_tx_stream : public baseband_gateway_transmitter, public uhd_exception_handler
{
private:
  /// Receive asynchronous message timeout in seconds.
  static constexpr double RECV_ASYNC_MSG_TIMEOUT_S = 0.001;
  /// Transmit timeout in seconds.
  static constexpr double TRANSMIT_TIMEOUT_S = 0.001;

  /// Indicates the stream identification for notifications.
  unsigned stream_id;
  /// Task executor for asynchronous messages.
  task_executor& async_executor;
  /// Radio notification interface.
  radio_notification_handler& notifier;
  /// USRP pointer, used for setting GPIO
  uhd::usrp::multi_usrp::sptr usrp;
  /// Owns the UHD Tx stream.
  uhd::tx_streamer::sptr stream;
  /// Maximum number of samples in a single packet.
  unsigned max_packet_size;
  /// Protects concurrent stream transmit.
  std::mutex stream_transmit_mutex;
  /// Sampling rate in Hz.
  double srate_hz;
  /// Number of samples to offset the tx/rx time by, used to compensate for
  /// radio offsets. Positive means start tx later/report rx as being later.
  int sample_offset;
  /// Indicates the number of channels.
  unsigned nof_channels;
  /// Indicates the current internal state.
  radio_uhd_tx_stream_fsm state_fsm;
  /// What the last tdd state according to tdd_config is
  bool last_tdd_config_state_tx = false;
  /// What the last transmite state is according to start/end padding
  bool last_sample_state_tx = false;
  /// Discontinous transmission mode flag.
  bool discontinuous_tx;
  /// Use only config for determining TX/RX
  bool discontinuous_config;
  /// GPIO transmission mode flag, i.e. set GPIO bits when transmitting/not
  bool gpio_tx;
  /// Number of samples to advance the burst start to protect agains power ramping effects.
  unsigned power_ramping_nof_samples;
  /// Which bit of the GPIO should be toggled for TX
  std::optional<unsigned> gpio_tx_index;
  /// True if gpio should be high for TX, false if gpio should be low for TX
  bool gpio_tx_sense;
  /// True if gpio should be set based off config, false otherwise
  bool gpio_tx_use_config;
  /// Amount of time to put GPIO in TX mode early in samples.
  unsigned gpio_tx_prelude_samples;
  /// Stores the time of the last transmitted sample.
  uhd::time_spec_t last_tx_timespec;
  /// Power ramping transmit buffer. It is filled with zeros, used to absorb power ramping when starting a transmission.
  baseband_gateway_buffer_dynamic power_ramping_buffer;

  /// Receive asynchronous message.
  void recv_async_msg();

  /// Runs the asynchronous message reception through the asynchronous task executor.
  void run_recv_async_msg();

  /// \brief Transmits a single baseband block.
  /// \param[out] nof_txd_samples Number of transmitted samples.
  /// \param[in] data             Buffer to transmit.
  /// \param[in] offset           Sample offset in the transmit buffers.
  /// \param[in] md               Transmission metadata.
  /// \return True if no exception is caught in the transmission process, false otherwise.
  bool transmit_block(unsigned&                             nof_txd_samples,
                      const baseband_gateway_buffer_reader& data,
                      unsigned                              offset,
                      const uhd::tx_metadata_t&             md);

public:
  /// Describes the necessary parameters to create an UHD transmit stream.
  struct stream_description {
    /// Identifies the stream.
    unsigned id;
    /// Over-the-wire format.
    radio_configuration::over_the_wire_format otw_format;
    /// Sampling rate in Hz.
    double srate_hz;
    /// Number of samples to offset the tx/rx time by, used to compensate for
    /// radio offsets. Positive means start tx later/report rx as being later.
    int sample_offset;
    /// Stream arguments.
    std::string args;
    /// Indicates the port indexes for the stream.
    std::vector<size_t> ports;
    /// Enables discontinuous transmission mode.
    bool discontinuous_tx;
    /// Use only config for determining TX/RX
    bool discontinuous_config;
    /// Time by which to advance the burst start, using zero padding to protect against power ramping.
    float power_ramping_us;
    /// If set, which bit of the GPIO should be toggled for TX
    std::optional<unsigned> gpio_tx_index;
    /// True if gpio should be high for TX, false if gpio should be low for TX
    bool gpio_tx_sense;
    /// True if gpio should be set based off config, false otherwise
    bool gpio_tx_use_config;
    /// Amount of time to put GPIO in TX mode early in microseconds.
    float gpio_tx_prelude;
  };

  /// \brief Constructs an UHD transmit stream.
  /// \param[in] usrp Provides the USRP context.
  /// \param[in] description Provides the stream configuration parameters.
  /// \param[in] async_executor_ Provides the asynchronous task executor.
  /// \param[in] notifier_ Provides the radio event notification handler.
  radio_uhd_tx_stream(uhd::usrp::multi_usrp::sptr& usrp_,
                      const stream_description&    description,
                      task_executor&               async_executor_,
                      radio_notification_handler&  notifier_);

  /// Gets the optimal transmitter buffer size.
  unsigned get_buffer_size() const;

  // See interface for documentation.
  void transmit(const baseband_gateway_buffer_reader&        data,
                const baseband_gateway_transmitter_metadata& metadata) override;

  /// Stop the transmission.
  void stop();

  /// Wait until radio is notify_stop.
  void wait_stop();
};
} // namespace srsran
