/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <functional> // for std::function/std::bind
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <queue>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#include "srsgnb/gateways/network_gateway_factory.h"

using namespace srsgnb;

byte_buffer make_tx_byte_buffer(uint32_t length)
{
  byte_buffer pdu{};
  for (uint32_t i = 0; i < length; ++i) {
    pdu.append((uint8_t)i);
  }
  return pdu;
}

byte_buffer make_small_tx_byte_buffer()
{
  return make_tx_byte_buffer(15);
}

byte_buffer make_large_tx_byte_buffer()
{
  return make_tx_byte_buffer(9000);
}

byte_buffer make_oversized_tx_byte_buffer()
{
  return make_tx_byte_buffer(9500);
}

class dummy_network_gateway_control_notifier : public network_gateway_control_notifier
{
public:
  dummy_network_gateway_control_notifier() = default;

  void on_connection_loss() override { conn_dropped = true; }
  void on_connection_established() override { conn_established = true; }
  bool get_connection_dropped() { return conn_dropped; }
  bool get_connection_established() { return conn_established; }

private:
  bool conn_established = false;
  bool conn_dropped     = false;
};

class dummy_network_gateway_data_notifier : public network_gateway_data_notifier
{
public:
  dummy_network_gateway_data_notifier() = default;
  void on_new_pdu(byte_buffer pdu) override
  {
    // printf("Received PDU\n");
    rx_bytes += pdu.length();
    pdu_queue.push(std::move(pdu));
  }

  unsigned get_rx_bytes() { return rx_bytes; }

  std::queue<byte_buffer> pdu_queue;

private:
  unsigned rx_bytes = 0;
};

class dummy_network_gateway_notifier : public network_gateway_control_notifier, public network_gateway_data_notifier
{
public:
  dummy_network_gateway_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_connection_loss() override { logger.info("Connection loss"); }
  void on_connection_established() override { logger.info("Connection established"); }
  void on_new_pdu(byte_buffer pdu) override { logger.info("Received PDU"); }

private:
  srslog::basic_logger& logger;
};

/// Dummy PDU handler
class dummy_network_gateway_data_handler : public network_gateway_data_handler
{
public:
  dummy_network_gateway_data_handler(){};
  void handle_pdu(const byte_buffer& pdu) override { last_pdu = pdu.copy(); }

  byte_buffer last_pdu;
};
