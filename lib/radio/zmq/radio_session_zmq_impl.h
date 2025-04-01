/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_zmq_baseband_gateway.h"
#include "radio_zmq_rx_stream.h"
#include "radio_zmq_tx_stream.h"
#include "srsran/radio/radio_session.h"
#include "srsran/support/executors/task_executor.h"
#include <zmq.h>

namespace srsran {

/// Describes a ZeroMQ radio based session.
class radio_session_zmq_impl : public radio_session, public radio_management_plane
{
private:
  /// Default sockets send and receive timeout in milliseconds.
  static constexpr unsigned DEFAULT_TRX_TIMEOUT_MS = 10;
  /// Default linger timeout in milliseconds.
  static constexpr unsigned DEFAULT_LINGER_TIMEOUT_MS = 0;
  /// Default stream buffer size in samples.
  static constexpr unsigned DEFAULT_STREAM_BUFFER_SIZE = 614400;

  /// Radio session logger.
  srslog::basic_logger& logger;
  /// ZMQ context.
  void* zmq_context;
  /// Stores transmit streams.
  std::vector<std::unique_ptr<radio_zmq_baseband_gateway>> bb_gateways;
  /// Indicates the session has been created succesfully.
  bool successful = false;

public:
  /// \brief Default constructor.
  /// \param[in] config Provides the required parameters to start a ZMQ radio based session.
  /// \param[in] async_task_executor Provides a task executor to perform asynchronous tasks.
  /// \param[in] notification_handler Provides a radio event notification handler.
  /// \note Use is_successful() to check that the instance was successfully initialized.
  radio_session_zmq_impl(const radio_configuration::radio& config,
                         task_executor&                    async_task_executor,
                         radio_notification_handler&       notification_handler);

  /// Default destructor.
  ~radio_session_zmq_impl();

  /// Indicates if the instance was successfully initialized.
  bool is_successful() const { return successful; }

  // See interface for documentation.
  radio_management_plane& get_management_plane() override { return *this; }

  // See interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    srsran_assert(stream_id < bb_gateways.size(),
                  "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {})",
                  stream_id,
                  bb_gateways.size());
    return *bb_gateways[stream_id];
  }

  // See interface documentation.
  baseband_gateway_timestamp read_current_time() override;

  // See interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override;

  // See interface for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override;
};

} // namespace srsran
