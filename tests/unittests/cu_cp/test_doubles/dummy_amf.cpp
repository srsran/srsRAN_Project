/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dummy_amf.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

class manual_dummy_amf : public amf_test_stub
{
public:
  explicit manual_dummy_amf() : rx_pdus(1024), pending_tx_pdus(16) {}

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

class auto_dummy_amf : public manual_dummy_amf
{
public:
  auto_dummy_amf() {}

  void on_new_message(const ngap_message& msg) override
  {
    if (not ng_is_setup) {
      report_fatal_error_if_not(is_pdu_type(msg, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request),
                                "Unexpected message received");
      ng_is_setup = true;
      this->push_tx_pdu(generate_ng_setup_response());
      return;
    }

    // Let parent class handle the message.
    manual_dummy_amf::on_new_message(msg);
  }

private:
  bool ng_is_setup = false;
};

std::unique_ptr<amf_test_stub> srsran::srs_cu_cp::create_manual_amf_stub()
{
  return std::make_unique<manual_dummy_amf>();
}

std::unique_ptr<amf_test_stub> srsran::srs_cu_cp::create_amf_stub()
{
  return std::make_unique<auto_dummy_amf>();
}