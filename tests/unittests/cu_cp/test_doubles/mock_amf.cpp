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

#include "mock_amf.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

/// \brief Mock class for the interface between DU and CU-CP that accounts for the fact that the CU-CP may push PDUs
/// from different threads.
class synchronized_mock_du : public mock_amf
{
public:
  explicit synchronized_mock_du() : rx_pdus(1024), pending_tx_pdus(16) {}

  void attach_cu_cp_pdu_handler(ngap_message_handler& cu_cp_) override { cu_cp_pdu_handler = &cu_cp_; }

  void on_new_message(const ngap_message& msg) override
  {
    // If a PDU response has been previously enqueued, we send it now.
    if (not pending_tx_pdus.empty()) {
      ngap_message tx_pdu;
      pending_tx_pdus.try_pop(tx_pdu);
      cu_cp_pdu_handler->handle_message(tx_pdu);
    }

    bool success = rx_pdus.push_blocking(msg);
    report_error_if_not(success, "Queue is full");
  }

  bool try_pop_rx_pdu(ngap_message& pdu) override { return rx_pdus.try_pop(pdu); }

  void push_tx_pdu(const ngap_message& pdu) override { cu_cp_pdu_handler->handle_message(pdu); }

  void enqueue_next_tx_pdu(const ngap_message& pdu) override { pending_tx_pdus.push_blocking(pdu); }

private:
  using ngap_pdu_queue = concurrent_queue<ngap_message,
                                          concurrent_queue_policy::locking_mpmc,
                                          concurrent_queue_wait_policy::condition_variable>;

  ngap_pdu_queue rx_pdus;

  ngap_message_handler* cu_cp_pdu_handler = nullptr;

  // Tx PDUs to send once the NG connection is set up.
  ngap_pdu_queue pending_tx_pdus;
};

std::unique_ptr<mock_amf> srsran::srs_cu_cp::create_mock_amf()
{
  return std::make_unique<synchronized_mock_du>();
}
