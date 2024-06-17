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

#include "apps/gnb/adapters/e2ap_adapter.h"
#include "lib/e2/common/e2ap_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "srsran/gateways/sctp_network_server_factory.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/error_handling.h"

namespace srsran {

class dlt_pcap;
class io_broker;

struct e2_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the E2AP messages.
  dlt_pcap& pcap;
  /// Sniffer that receives a copy of a received E2 message.
  e2_message_notifier* rx_sniffer;
};

class ric_e2_handler
{
public:
  virtual ~ric_e2_handler() = default;
  virtual std::unique_ptr<e2_message_notifier>
  handle_new_du_connection(std::unique_ptr<e2_message_notifier> e2_tx_pdu_notifier) = 0;
};

/// Connection server responsible for handling new E2 connection.
class e2_connection_server
{
public:
  virtual ~e2_connection_server() = default;

  /// Attach a RIC handler to the E2 connection server.
  virtual void attach_ric(ric_e2_handler& ric_) = 0;

  /// Get port on which the E2 Server is listening for new connections.
  ///
  /// This method is useful in testing, where we don't want to use a specific port.
  /// \return The port number on which the E2 Server is listening for new connections.
  virtual std::optional<uint16_t> get_listen_port() const = 0;
};

/// Creates a RIC Gateway server that listens for incoming SCTP connections, packs/unpacks E2AP PDUs and forwards
/// them to the GW/RIC E2AP handler.
std::unique_ptr<e2_connection_server> create_e2_gateway_server(const e2_sctp_gateway_config& params);

struct e2_agent_repository {
  std::map<unsigned, std::unique_ptr<e2_message_notifier>> db;
};

class e2_agent_connection_manager : public ric_e2_handler
{
public:
  e2_agent_connection_manager(e2_agent_repository& e2_agents_);

  /// Handle a new connection from E2 Agent.
  std::unique_ptr<e2_message_notifier>
  handle_new_du_connection(std::unique_ptr<e2_message_notifier> e2_tx_pdu_notifier) override;

private:
  class e2_gw_to_ric_pdu_adapter;
  unsigned get_next_e2_agent_index();

  srslog::basic_logger& logger;
  e2_agent_repository&  e2_agents;
};

class near_rt_ric
{
public:
  near_rt_ric();

  ric_e2_handler& get_ric_e2_handler() { return e2_agent_mng; }

  /// RIC sends msg to E2 Agent
  void send_msg(unsigned e2_agent_idx, const e2_message& msg);

private:
  srslog::basic_logger&       logger;
  e2_agent_connection_manager e2_agent_mng;
  e2_agent_repository         e2_agents;
};

} // namespace srsran