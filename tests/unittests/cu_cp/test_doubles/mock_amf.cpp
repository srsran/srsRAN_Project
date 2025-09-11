/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/adt/mutexed_mpmc_queue.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/srslog/srslog.h"
#include <atomic>

using namespace srsran;
using namespace srs_cu_cp;

/// \brief Mock class for the interface between CU-CP and AMF that accounts for the fact that the CU-CP may push PDUs
/// from different threads.
class synchronized_mock_amf : public mock_amf
{
public:
  explicit synchronized_mock_amf() : rx_pdus(1024), pending_tx_pdus(16) {}

  std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_rx_message_notifier> cu_cp_rx_pdu_notifier) override
  {
    class sync_mock_pdu_notifier : public ngap_message_notifier
    {
    public:
      sync_mock_pdu_notifier(synchronized_mock_amf& parent_) : parent(parent_) {}
      ~sync_mock_pdu_notifier() override { parent.rx_pdu_notifier.reset(); }

      [[nodiscard]] bool on_new_message(const ngap_message& msg) override
      {
        // If a NG Reset is sent, we inject a NG Reset Acknowledge message.
        if (msg.pdu.type().value == asn1::ngap::ngap_pdu_c::types_opts::init_msg &&
            msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_NG_RESET) {
          auto& ng_reset = msg.pdu.init_msg().value.ng_reset();

          ngap_message ng_reset_ack = generate_ng_reset_ack(
              (ng_reset->reset_type.type() == asn1::ngap::reset_type_c::types_opts::options::part_of_ng_interface)
                  ? ng_reset->reset_type.part_of_ng_interface()
                  : asn1::ngap::ue_associated_lc_ng_conn_list_l{});
          parent.push_tx_pdu(ng_reset_ack);
        }

        // If a PDU response has been previously enqueued, we send it now.
        if (not parent.pending_tx_pdus.empty()) {
          ngap_message tx_pdu;
          bool         discard = parent.pending_tx_pdus.try_pop(tx_pdu);
          (void)discard;
          parent.push_tx_pdu(tx_pdu);
        }

        bool success = parent.rx_pdus.push_blocking(msg);
        report_error_if_not(success, "Queue is full");
        return true;
      }

    private:
      synchronized_mock_amf& parent;
    };

    if (connection_dropped) {
      srslog::fetch_basic_logger("TEST").warning("AMF connection already dropped");
      return nullptr;
    }

    // Ensure any existing notifier is properly reset before assigning a new one.
    if (rx_pdu_notifier) {
      rx_pdu_notifier.reset();
    }

    rx_pdu_notifier = std::move(cu_cp_rx_pdu_notifier);
    return std::make_unique<sync_mock_pdu_notifier>(*this);
  }

  bool try_pop_rx_pdu(ngap_message& pdu) override { return rx_pdus.try_pop(pdu); }

  void push_tx_pdu(const ngap_message& pdu) override { rx_pdu_notifier->on_new_message(pdu); }

  void enqueue_next_tx_pdu(const ngap_message& pdu) override { pending_tx_pdus.push_blocking(pdu); }

  void drop_connection() override
  {
    connection_dropped = true;
    if (rx_pdu_notifier) {
      rx_pdu_notifier.reset();
    }
  }

private:
  using ngap_pdu_queue = concurrent_queue<ngap_message,
                                          concurrent_queue_policy::locking_mpmc,
                                          concurrent_queue_wait_policy::condition_variable>;

  ngap_pdu_queue rx_pdus;

  std::atomic<bool> connection_dropped = false;

  std::unique_ptr<ngap_rx_message_notifier> rx_pdu_notifier;

  // Tx PDUs to send once the NG connection is set up.
  ngap_pdu_queue pending_tx_pdus;
};

std::unique_ptr<mock_amf> srsran::srs_cu_cp::create_mock_amf()
{
  return std::make_unique<synchronized_mock_amf>();
}
