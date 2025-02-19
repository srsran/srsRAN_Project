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

#include "srsran/ngap/gateways/n2_connection_client.h"
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

/// Interface to AMF node that can be accessed safely from the test main thread.
class mock_amf : public n2_connection_client
{
public:
  /// Pop PDU sent by CU-CP and received by AMF.
  virtual bool try_pop_rx_pdu(ngap_message& pdu) = 0;

  /// Push new Tx PDU from AMF to CU-CP.
  virtual void push_tx_pdu(const ngap_message& pdu) = 0;

  /// Enqueue an NGAP PDU that the AMF will automatically send as a response to the next CU-CP Tx PDU.
  virtual void enqueue_next_tx_pdu(const ngap_message& pdu) = 0;

  /// Simulate a drop of the AMF connection.
  virtual void drop_connection() = 0;
};

/// Creates a mock AMF to interface with the CU-CP.
std::unique_ptr<mock_amf> create_mock_amf();

} // namespace srs_cu_cp
} // namespace srsran