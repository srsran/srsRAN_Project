/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_uhd_exception_handler.h"
#include "radio_uhd_multi_usrp.h"
#include "radio_uhd_tx_stream_fsm.h"
#include "srsgnb/gateways/baseband/baseband_gateway_buffer.h"
#include "srsgnb/radio/radio_configuration.h"
#include "srsgnb/radio/radio_notification_handler.h"
#include "srsgnb/support/executors/task_executor.h"
#include <mutex>

namespace srsgnb {
class radio_uhd_tx_stream : public uhd_exception_handler
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
  /// Owns the UHD Tx stream.
  uhd::tx_streamer::sptr stream;
  /// Protects concurrent stream transmit.
  std::mutex stream_transmit_mutex;
  /// Sampling rate in Hz.
  double srate_hz;
  /// Indicates the number of channels.
  unsigned nof_channels;
  /// Indicates the current internal state.
  radio_uhd_tx_stream_fsm state_fsm;

  /// Receive asynchronous message.
  void recv_async_msg();

  /// Runs the asynchronous message reception through the asynchronous task executor.
  void run_recv_async_msg();

  /// \brief Transmits a single baseband block.
  /// \param[out] nof_txd_samples Number of transmitted samples.
  /// \param[in] data Provides the buffers tpo transmit.
  /// \param[in] offset Indicates the sample offset in the transmit buffers.
  /// \param[in] time_spec Indicates the transmission timestamp.
  /// \return True if no exception is caught in the transmission process. Otherwise, false.
  bool transmit_block(unsigned&                nof_txd_samples,
                      baseband_gateway_buffer& data,
                      unsigned                 offset,
                      uhd::time_spec_t&        time_spec);

public:
  /// Describes the necessary parameters to create an UHD transmit stream.
  struct stream_description {
    /// Identifies the stream.
    unsigned id;
    /// Over-the-wire format.
    radio_configuration::over_the_wire_format otw_format;
    /// Sampling rate in Hz.
    double srate_hz;
    /// Stream arguments.
    std::string args;
    /// Indicates the port indexes for the stream.
    std::vector<size_t> ports;
  };

  /// \brief Constructs an UHD transmit stream.
  /// \param[in] usrp Provides the USRP context.
  /// \param[in] description Provides the stream configuration parameters.
  /// \param[in] async_executor_ Provides the asynchronous task executor.
  /// \param[in] notifier_ Provides the radio event notification handler.
  radio_uhd_tx_stream(uhd::usrp::multi_usrp::sptr& usrp,
                      const stream_description&    description,
                      task_executor&               async_executor_,
                      radio_notification_handler&  notifier_);

  /// \brief Transmits baseband signal.
  /// \param[in] data Provides the baseband buffers to transmit.
  /// \param[in] time_spec Indicates the transmission time.
  /// \return True if the transmission was successful. Otherwise, false.
  bool transmit(baseband_gateway_buffer& data, uhd::time_spec_t time_spec);

  /// Stop the transmission.
  void stop();
};
} // namespace srsgnb
