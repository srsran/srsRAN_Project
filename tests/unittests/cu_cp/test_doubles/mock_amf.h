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

#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

/// Interface to AMF node that can be accessed safely from the test main thread.
class mock_amf : public ngap_message_notifier
{
public:
  virtual void attach_cu_cp_pdu_handler(ngap_message_handler& cu_cp_) = 0;

  /// Pop PDU sent by CU-CP and received by AMF.
  virtual bool try_pop_rx_pdu(ngap_message& pdu) = 0;

  /// Push new Tx PDU from AMF to CU-CP.
  virtual void push_tx_pdu(const ngap_message& pdu) = 0;

  /// Enqueue an NGAP PDU that the AMF will automatically send as a response to the next CU-CP Tx PDU.
  virtual void enqueue_next_tx_pdu(const ngap_message& pdu) = 0;
};

/// Creates a mock AMF to interface with the CU-CP.
std::unique_ptr<mock_amf> create_mock_amf();

} // namespace srs_cu_cp
} // namespace srsran