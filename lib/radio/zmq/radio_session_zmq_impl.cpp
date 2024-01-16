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

#include "radio_session_zmq_impl.h"

using namespace srsran;

radio_session_zmq_impl::radio_session_zmq_impl(const radio_configuration::radio& config,
                                               task_executor&                    async_task_executor,
                                               radio_notification_handler&       notifier) :
  logger(srslog::fetch_basic_logger("RF", false))
{
  // Make sure the number of streams are equal.
  srsran_assert(config.tx_streams.size() == config.rx_streams.size(),
                "The number of transmit streams (i.e., {}) must be equal to the number of receive streams (i.e., {}).",
                config.tx_streams.size(),
                config.rx_streams.size());

  // Make ZMQ context.
  zmq_context = zmq_ctx_new();
  if (zmq_context == nullptr) {
    logger.error("Failed to create ZMQ context. {}.", zmq_strerror(zmq_errno()));
    return;
  }

  unsigned nof_streams = config.tx_streams.size();

  // Debug log level is only available if verbose keyword is in the device arguments.
  bool allow_log_level_debug = (config.args.find("verbose") != std::string::npos);

  // ZMQ logging in debug is extremely verbose. The following lines avoid debug level unless set to paranoid.
  srslog::basic_levels log_level = srslog::str_to_basic_level(config.log_level);
  if (!allow_log_level_debug && (log_level >= srslog::basic_levels::debug)) {
    log_level = srslog::basic_levels::info;
  }

  // Iterate for each transmission and reception stream.
  for (unsigned stream_id = 0; stream_id != nof_streams; ++stream_id) {
    const radio_configuration::stream& tx_radio_stream_config = config.tx_streams[stream_id];

    // Prepare transmit stream configuration.
    radio_zmq_tx_stream::stream_description tx_stream_config;
    tx_stream_config.socket_type = ZMQ_REP;
    for (unsigned channel_id = 0; channel_id != tx_radio_stream_config.channels.size(); ++channel_id) {
      tx_stream_config.address.push_back(tx_radio_stream_config.channels[channel_id].args);
    }
    tx_stream_config.stream_id         = stream_id;
    tx_stream_config.stream_id_str     = "zmq:tx:" + std::to_string(stream_id);
    tx_stream_config.log_level         = log_level;
    tx_stream_config.trx_timeout_ms    = DEFAULT_TRX_TIMEOUT_MS;
    tx_stream_config.linger_timeout_ms = DEFAULT_LINGER_TIMEOUT_MS;
    tx_stream_config.buffer_size       = DEFAULT_STREAM_BUFFER_SIZE;

    const radio_configuration::stream& rx_radio_stream_config = config.rx_streams[stream_id];

    // Prepare transmit stream configuration.
    radio_zmq_rx_stream::stream_description rx_stream_config;
    rx_stream_config.socket_type = ZMQ_REQ;
    for (unsigned channel_id = 0; channel_id != rx_radio_stream_config.channels.size(); ++channel_id) {
      rx_stream_config.address.push_back(rx_radio_stream_config.channels[channel_id].args);
    }
    rx_stream_config.stream_id         = stream_id;
    rx_stream_config.stream_id_str     = "zmq:rx:" + std::to_string(stream_id);
    rx_stream_config.log_level         = log_level;
    rx_stream_config.trx_timeout_ms    = DEFAULT_TRX_TIMEOUT_MS;
    rx_stream_config.linger_timeout_ms = DEFAULT_LINGER_TIMEOUT_MS;
    rx_stream_config.buffer_size       = DEFAULT_STREAM_BUFFER_SIZE;

    // Create baseband gateway.
    bb_gateways.emplace_back(std::make_unique<radio_zmq_baseband_gateway>(
        zmq_context, async_task_executor, notifier, tx_stream_config, rx_stream_config));

    // Make sure streams are created successfully.
    if (!bb_gateways.back()->get_tx_stream().is_successful() || !bb_gateways.back()->get_rx_stream().is_successful()) {
      return;
    }
  }

  successful = true;
}

radio_session_zmq_impl::~radio_session_zmq_impl()
{
  // Destroy transmit and receive streams prior to ZMQ context destruction.
  bb_gateways.clear();

  // Destroy ZMQ context.
  if (zmq_context != nullptr) {
    zmq_ctx_shutdown(zmq_context);
    zmq_ctx_destroy(zmq_context);
    zmq_context = nullptr;
  }
}

void radio_session_zmq_impl::stop()
{
  // Signal stop for each transmit stream.
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().stop();
  }

  // Signal stop for each receive stream.
  for (auto& gateway : bb_gateways) {
    gateway->get_rx_stream().stop();
  }

  // Wait for streams to join.
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().wait_stop();
    gateway->get_rx_stream().wait_stop();
  }
}

bool radio_session_zmq_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return false;
}

bool radio_session_zmq_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return false;
}

void radio_session_zmq_impl::start(baseband_gateway_timestamp start_time)
{
  for (auto& gateway : bb_gateways) {
    gateway->get_rx_stream().start(start_time);
  }
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().start(start_time);
  }
}

baseband_gateway_timestamp radio_session_zmq_impl::read_current_time()
{
  return 0;
}
